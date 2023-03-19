/*
   TODO:
     * Error handling for invalid posts
 */

// Prism.js normally highlights on page render
// In order to get it to highlight dynamically loaded content it needs to be called
[@bs.module "prismjs"] external highlightAll: unit => unit = "highlightAll";

[@bs.module "js-yaml"] external parseYaml: string => Js.t({..}) = "load";

// TODO: This is probably not the most efficient way to load things
//       Wonder how hard it would be to move these calls to only happen when a particular language is detected?
//       Is there a better way than raw js calls?
%raw
"require('prismjs/components/prism-reason');";
%raw
"require('prismjs/components/prism-python');";
%raw
"require('prismjs/components/prism-gdscript');";

%raw
"require('prismjs/plugins/line-numbers/prism-line-numbers');";

let contentRoot = "https://api.github.com/repos/trite/trite.io-content/contents/";

let postsContentRoot = contentRoot ++ "posts/";

let (+/) = (left, right) =>
  switch (
    left |> String.endsWith(~search="/"),
    right |> String.startsWith(~search="/"),
  ) {
  | (false, false) => left ++ "/" ++ right
  | (true, true) => String.slice(0, left |> String.length, left) ++ right
  | _ => left ++ right
  };

let makeUri = path => contentRoot ++ path;

let makePostsUri = (~folder, path) =>
  postsContentRoot ++ folder +/ path ++ ".md";

let getValue = e => (e |> ReactEvent.Form.target)##value;

let foldUnitResults = Result.fold(() => (), () => ());

module Styles = {
  open Css;

  let container = style([position(relative), display(grid)]);

  let centeredElement =
    style([width(pct(50.0)), justifySelf(center), position(absolute)]);
};

[@react.component]
let make = (~folder, ~post) => {
  module S = Components.S;

  let (postContent, setPostContent) = React.useState(() => "");

  let (testData, setTestData) = React.useState(() => "");

  post
  |> makePostsUri(~folder)
  |> ContentFetch.fetchString
  |> IO.map(
       Js.Json.parseExn
       >> GithubApi.decode
       >> Result.fold(
            err =>
              setPostContent(_ =>
                err |> Decode.ParseError.failureToDebugString
              ),
            ({content_decoded, _}: GithubApi.githubApiResponse) =>
              Parsing.(
                {
                  let {meta, data} = content_decoded |> parseContent;
                  let parsedMeta = meta |> parseYaml;
                  let title =
                    parsedMeta##title
                    |> Js.toOption
                    |> Option.getOrElse("No Title Provided");
                  let created =
                    parsedMeta##created
                    |> Js.toOption
                    |> Option.map(Js.Date.toLocaleDateString)
                    |> Option.getOrElse("Unknown");
                  setPostContent(_ => data);
                  highlightAll();
                  setTestData(_ => {j|Title: $title\nCreated: $created|j});
                }
              ),
          ),
     )
  |> IO.mapError(error =>
       setPostContent(_ => error |> ContentFetch.Error.show)
     )
  |> IO.unsafeRunAsync(foldUnitResults);

  <div className=Styles.centeredElement>
    <S> {"Fetching: " ++ (post |> makePostsUri(~folder))} </S>
    <br />
    <textarea value=testData />
    <br />
    <div dangerouslySetInnerHTML={"__html": postContent} />
  </div>;
};
