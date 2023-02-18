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

type links = {
  self: string,
  git: string,
  html: string,
};

type githubApiResponse = {
  name: string,
  path: string,
  sha: string,
  size: int,
  url: string,
  html_url: string,
  git_url: string,
  download_url: string,
  type_: string,
  content: string,
  encoding: string,
  _links: links,
};

let githubApiResponseToString =
    (
      {
        name,
        path,
        sha,
        size,
        url,
        html_url,
        git_url,
        download_url,
        type_,
        content,
        encoding,
        _links: {self, git, html},
      },
    ) => {j|
  name:         $name
  path:         $path
  sha:          $sha
  size:         $size
  url:          $url
  html_url:     $html_url
  git_url:      $git_url
  download_url: $download_url
  type:         $type_
  content:      $content
  encoding:     $encoding
  ----links----
    self: $self
    git:  $git
    html: $html
|j};

let linksMake = (self, git, html) => {self, git, html};

let githubApiMake =
    (
      name,
      path,
      sha,
      size,
      url,
      html_url,
      git_url,
      download_url,
      type_,
      content,
      encoding,
      _links,
    ) => {
  name,
  path,
  sha,
  size,
  url,
  html_url,
  git_url,
  download_url,
  type_,
  content,
  encoding,
  _links,
};

let decodeLinks = (json): result(links, Decode_ParseError.failure) =>
  Decode.AsResult.OfParseError.Pipeline.(
    succeed(linksMake)
    |> field("self", string)
    |> field("git", string)
    |> field("html", string)
    |> run(json)
  );

let decodeTest = json =>
  Decode.AsResult.OfParseError.Pipeline.(
    succeed(githubApiMake)
    |> field("name", string)
    |> field("path", string)
    |> field("sha", string)
    |> field("size", intFromNumber)
    |> field("url", string)
    |> field("html_url", string)
    |> field("git_url", string)
    |> field("download_url", string)
    |> field("type", string)
    |> field("content", string)
    |> field("encoding", string)
    |> field("_links", decodeLinks)
    |> run(json)
  );

module App = {
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
      <input
        type_="button"
        value="go forth and fetch the thing"
        onClick={doSpecificFetch(specificFetch, setSpecificFetchResult)}
      />
      <br />
      <pre> <S> specificFetchResult </S> </pre>
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
