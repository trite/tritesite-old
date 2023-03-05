open Bindings;

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

module Styles = {
  open Css;

  let wideTextInput = style([width(pct(50.0))]);

  let wideTextArea = style([width(pct(50.0)), height(px(150))]);
};

let getValue = e => e->ReactEvent.Form.target##value;

let parseAndSet = (setter, event) =>
  event |> getValue |> parse |> sanitize |> (x => setter(_ => x));

let foldUnitResults = Result.fold(() => (), () => ());

let fetch = (setter, uri, _event) =>
  ContentFetch.fetchString(uri)
  |> IO.map(content => setter(_ => content))
  |> IO.mapError(error => setter(_ => error |> ContentFetch.Error.show))
  |> IO.unsafeRunAsync(foldUnitResults);

[@react.component]
let make = (~folder, ~post) => {
  module S = Components.S;

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
            ({content_decoded, _}: GithubApi.githubApiResponse) =>
              setPostContent(_ =>
                content_decoded |> Bindings.parse |> Bindings.sanitize
              ),
          ),
     )
  |> IO.mapError(error =>
       setPostContent(_ => error |> ContentFetch.Error.show)
     )
  |> IO.unsafeRunAsync(foldUnitResults);

  <div>
    <S> {"Fetching: " ++ (post |> makePostsUri(~folder))} </S>
    <br />
    <hr />
    <br />
    <div dangerouslySetInnerHTML={"__html": postContent} />
  </div>;
};
