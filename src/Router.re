let handleRoutes = urlPath => {
  module S = Components.S;
  switch (urlPath) {
  | []
  | ["home"] => <Home />
  | ["posts", folder, post] => <Posts folder post />
  // | ["posts", folder, post] =>
  //   <div> <S> {j|folder: $folder, post: $post|j} </S> </div>
  | _ =>
    <div>
      <S> "404 - invalid url" </S>
      <br />
      <S> {"[" ++ (urlPath |> List.String.joinWith(", ")) ++ "]"} </S>
    </div>
  };
};

module App = {
  [@react.component]
  let make = () => {
    let url = ReasonReactRouter.useUrl();

    handleRoutes(url.path);
  };
};

ReactDOM.querySelector("#root")
|> (
  fun
  | Some(root) => ReactDOM.render(<App />, root)
  | None =>
    Js.Console.error("Failed to start React: couldn't find the #root element")
);
