/*
   TODO:
     * Error handling for invalid posts
 */

// %raw
// "import Prism from 'prismjs';";

[@bs.module "prismjs"] external highlightAll: unit => unit = "highlightAll";
// [@bs.module "prismjs/components/"] /*[@bs.scope "components"]*/
// external loadLanguages: list(string) => unit = "loadLanguages";

// %raw
// "import Prism from 'prismjs';";
// %raw
// "import \"prismjs/components/prism-reason\";";
%raw
"require('prismjs/components/prism-reason');";
%raw
"require('prismjs/components/prism-python');";
%raw
"require('prismjs/components/prism-gdscript');";

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

let getValue = e => e->ReactEvent.Form.target##value;

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

  // loadLanguages(["reason"]);

  let (postContent, setPostContent) = React.useState(() => "");

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
            ({content_decoded, _}: GithubApi.githubApiResponse) => {
              setPostContent(_ =>
                content_decoded |> Bindings.parse |> Bindings.sanitize
              );
              highlightAll();
            },
          ),
     )
  |> IO.mapError(error =>
       setPostContent(_ => error |> ContentFetch.Error.show)
     )
  |> IO.unsafeRunAsync(Result.fold(() => highlightAll(), () => ()));

  // highlightAll();

  <div className=Styles.container>
    <div className=Styles.centeredElement>
      <S> {"Fetching: " ++ (post |> makePostsUri(~folder))} </S>
      <br />
      <div dangerouslySetInnerHTML={"__html": postContent} />
    </div>
  </div>;
};
