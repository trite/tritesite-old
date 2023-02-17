[@bs.module "marked"] external parse: string => string = "parse";

[@bs.module "dompurify"] external sanitize: string => string = "sanitize";

let contentRoot = "https://api.github.com/repos/trite/trite.io-content/contents/";

let makeUri = path => contentRoot ++ path;

let defaultRawMarkdown = {|# header
## subheader
Text **bold** *italic*
* list
* of
* things|};

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

module App = {
  let getValue = e => e->ReactEvent.Form.target##value;

  let parseAndSet = (setter, event) =>
    event |> getValue |> parse |> sanitize |> (x => setter(_ => x));

  let fetch = (setter, uri, _event) =>
    ContentFetch.fetchString(uri)
    |> IO.unsafeRunAsync(
         fun
         | Ok(content) => setter(_ => content)
         | Error(error) => setter(_ => error |> ContentFetch.Error.show),
       );

  [@react.component]
  let make = () => {
    let (parsedMarkdown, setParsedMarkdown) = React.useState(() => "");

    let (fetched, setFetched) = React.useState(() => "");

    let (specificFetch, setSpecificFetch) =
      React.useState(() => "posts/2022/2022-01-18_python-type-checking.md");

    let (specificFetchResult, setSpecificFetchResult) =
      React.useState(() => "");

    <div>
      <label> <S> "Markdown parse test: " </S> </label>
      <textarea
        onChange={parseAndSet(setParsedMarkdown)}
        className=Styles.wideTextArea
      />
      <br />
      <div dangerouslySetInnerHTML={"__html": parsedMarkdown} />
      <br />
      <hr />
      <br />
      <label> <S> "Specific fetch test: " </S> </label>
      <input
        type_="text"
        value=specificFetch
        onChange={getValue >> setSpecificFetch}
        className=Styles.wideTextInput
      />
      <br />
      <textarea className=Styles.wideTextArea />
      <input
        type_="button"
        value="go forth and fetch the thing"
        onClick={specificFetch |> makeUri |> fetch(setSpecificFetchResult)}
      />
      <br />
      <S> specificFetchResult </S>
      // <input type_="button" value="fetch & parse" onClick={}
      <br />
      <hr />
      <br />
      <label> <S> "Fetch test: " </S> </label>
      <textarea value=fetched className=Styles.wideTextArea />
      <input
        type_="button"
        value="dooo it"
        onClick={fetch(setFetched, contentRoot)}
      />
    </div>;
  };
};

ReactDOM.querySelector("#root")
|> (
  fun
  | Some(root) => ReactDOM.render(<App />, root)
  | None =>
    Js.Console.error("Failed to start React: couldn't find the #root element")
);
