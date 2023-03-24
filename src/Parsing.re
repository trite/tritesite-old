[@bs.module "marked"] external parse: string => string = "parse";

[@bs.module "dompurify"] external sanitize: string => string = "sanitize";

[@bs.module "js-yaml"]
external parseYaml: string => Js.Dict.t(string) = "load";

type metaData = {
  title: string,
  created: option(Js.Date.t),
};

type partiallyParsedContent = {
  rawMeta: string,
  rawData: string,
};

type parsedContent = {
  meta: metaData,
  data: string,
};

let frontMatterDelim = [%re {|/\n?---\n?/g|}];

let parseYaml = mightBeYaml => {
  let parsed = mightBeYaml |> parseYaml;

  let title =
    parsed |> Js.Dict.get(_, "title") |> Option.getOrElse("No Title Found");

  let created =
    parsed |> Js.Dict.get(_, "created") |> Option.map(Js.Date.fromString);

  {title, created};
};

let extractRawValues = (~fallback) =>
  fun
  | [Some(""), Some(rawMeta), Some(rawData)] => {rawMeta, rawData}
  | [Some(""), Some(rawMeta), ...data] => {
      rawMeta,
      rawData: data |> List.catOptions |> List.String.joinWith("\n---\n"),
    }
  | []
  | [_]
  | [_, _]
  | _ => {rawMeta: "", rawData: fallback};

let parseContent = rawInput =>
  rawInput
  |> Js.String.splitByRe(frontMatterDelim)
  |> Array.map(Option.map(Js.String.trim))
  |> Array.toList
  |> extractRawValues(~fallback=rawInput)
  |> (
    ({rawMeta, rawData}) => {
      meta: rawMeta |> parseYaml,
      data: rawData |> parse |> sanitize,
    }
  );
