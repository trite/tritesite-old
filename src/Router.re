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
  global(
    "body",
    [
      backgroundColor(hex("242424")),
      color(hex("ddd")),
      fontFamily(`custom("Inter")),
      fontSize(em(1.0)),
      lineHeight(`em(1.5)),
      margin(zero),
      padding(zero),
    ],
  );
  global("a:link", [color(yellow)]);
  global("a:visited", [color(green)]);
  global("a:hover", [color(orange)]);
  global("a:active", [color(white)]);

  let container = style([position(relative), display(grid)]);
};

module App = {
  [@react.component]
  let make = () => {
    <div className=Styles.container>
      {ReasonReactRouter.useUrl().path |> handleRoutes}
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
