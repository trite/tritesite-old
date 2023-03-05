/*
   TODO:
     * Error handling for invalid posts
 */

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
