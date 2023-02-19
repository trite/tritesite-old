[@bs.module "marked"] external parse: string => string = "parse";

[@bs.module "dompurify"] external sanitize: string => string = "sanitize";

let contentRoot = "https://api.github.com/repos/trite/trite.io-content/contents/";

let makeUri = path => contentRoot ++ path;

module ContentFetch = {
  module Error = {
    type t = ReludeFetch.Error.t(string);
    let show = error => ReludeFetch.Error.show(a => a, error);
    module Type = {
      type nonrec t = t;
    };
  };

  module IOE = IO.WithError(Error.Type);
  open IOE.Infix;

  let fetchString = uri =>
    ReludeFetch.fetch(uri) >>= ReludeFetch.Response.text;
};

module S = {
  [@react.component]
  let make = (~children) => children |> React.string;
};

module Styles = {
  open Css;

  let wideTextInput = style([width(pct(50.0))]);

  let wideTextArea = style([width(pct(50.0)), height(px(150))]);
};

// let url =

module App = {
  open GithubApi;

  let getValue = e => e->ReactEvent.Form.target##value;

  let parseAndSet = (setter, event) =>
    event |> getValue |> parse |> sanitize |> (x => setter(_ => x));

  let foldUnitResults = Result.fold(() => (), () => ());

  let fetch = (setter, uri, _event) =>
    ContentFetch.fetchString(uri)
    |> IO.map(content => setter(_ => content))
    |> IO.mapError(error => setter(_ => error |> ContentFetch.Error.show))
    |> IO.unsafeRunAsync(foldUnitResults);

  let doSpecificFetch = (specificFetch, setSpecificFetchResult, _event) =>
    specificFetch
    |> makeUri
    |> ContentFetch.fetchString
    |> IO.map(
         Js.Json.parseExn
         >> decodeTest
         >> Result.fold(
              err =>
                setSpecificFetchResult(_ =>
                  err |> Decode.ParseError.failureToDebugString
                ),
              githubApiResponseToString
              >> (x => setSpecificFetchResult(_ => x)),
            ),
       )
    |> IO.mapError(error =>
         setSpecificFetchResult(_ => error |> ContentFetch.Error.show)
       )
    |> IO.unsafeRunAsync(foldUnitResults);

  [@react.component]
  let make = () => {
    let (parsedMarkdown, setParsedMarkdown) = React.useState(() => "");

    let (fetched, setFetched) = React.useState(() => "");

    let (specificFetch, setSpecificFetch) =
      React.useState(() => "posts/2022/2022-01-18_python-type-checking.md");

    let (specificFetchResult, setSpecificFetchResult) =
      React.useState(() => "");

    <div> <span> <S> "blah" </S> </span> </div>;
    // <label> <S> "Markdown parse test: " </S> </label>
    // <textarea
    //   onChange={parseAndSet(setParsedMarkdown)}
    //   className=Styles.wideTextArea
    // />
    // <br />
    // <div dangerouslySetInnerHTML={"__html": parsedMarkdown} />
    // <br />
    // <hr />
    // <br />
    // <label> <S> "Specific fetch test: " </S> </label>
    // <input
    //   type_="text"
    //   value=specificFetch
    //   onChange={getValue >> setSpecificFetch}
    //   className=Styles.wideTextInput
    // />
    // <input
    //   type_="button"
    //   value="go forth and fetch the thing"
    //   onClick={doSpecificFetch(specificFetch, setSpecificFetchResult)}
    // />
    // <br />
    // <pre> <S> specificFetchResult </S> </pre>
    // <br />
    // <hr />
    // <br />
    // <label> <S> "Fetch test: " </S> </label>
    // <textarea value=fetched className=Styles.wideTextArea />
    // <input
    //   type_="button"
    //   value="dooo it"
    //   onClick={fetch(setFetched, contentRoot)}
    // />
  };
};

ReactDOM.querySelector("#root")
|> (
  fun
  | Some(root) => ReactDOM.render(<App />, root)
  | None =>
    Js.Console.error("Failed to start React: couldn't find the #root element")
);
