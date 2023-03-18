// %raw
// "import Prism from 'prismjs';";
// %raw
// "import \"prismjs/components/prism-reason\";";

let handleRoutes = urlPath => {
  module S = Components.S;

  switch (urlPath) {
  | []
  | ["home"] => <Home />
  | ["posts", folder, post] => <Posts folder post />
  | ["posts", ...rest] =>
    let path = rest |> List.String.joinWith("/");
    <div>
      <S>
        {j|You attempted to navigate to an invalid resource: tritesite.tld/posts/$path|j}
      </S>
      <br />
      <pre>
        {{j|Posts page requires a folder and post to work properly.
  Example: tritesite.tld/posts/2022/2022-12-18_reason-relude-custom-type-set
    Folder: 2022
    Post: 2022-12-18_reason-relude-custom-type-set|j}
         |> React.string}
      </pre>
    </div>;
  | _ =>
    <div>
      <S> "404 - invalid url" </S>
      <br />
      <S> {"[" ++ (urlPath |> List.String.joinWith(", ")) ++ "]"} </S>
    </div>
  };
};

module Styles = {
  open Css;

  global("html", [boxSizing(borderBox)]);
};

module App = {
  [@react.component]
  let make = () => {
    ReasonReactRouter.useUrl().path |> handleRoutes;
  };
};

ReactDOM.querySelector("#root")
|> (
  fun
  | Some(root) => ReactDOM.render(<App />, root)
  | None =>
    Js.Console.error("Failed to start React: couldn't find the #root element")
);
