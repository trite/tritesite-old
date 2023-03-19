[@bs.module "marked"] external parse: string => string = "parse";

[@bs.module "dompurify"] external sanitize: string => string = "sanitize";

type parsedContent = {
  meta: string,
  data: string,
};

let parseContent = rawInput =>
  rawInput
  |> String.splitList(~delimiter="---")
  |> (
    fun
    | [] => {meta: "", data: rawInput}
    | [_]
    | [_, _] => failwith("Figure this scenario out later")
    | ["", meta, data] => {meta, data}
    | ["", meta, ...data] => {
        meta,
        data: data |> List.String.joinWith("\n---\n"),
      }
    | _ => failwith("Figure this scenario out later")
  )
  |> (({meta, data}) => {meta, data: data |> parse |> sanitize});
