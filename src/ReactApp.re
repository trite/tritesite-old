[@bs.module "marked"] external parse: string => string = "parse";

module App = {
  let getValue = e => e->ReactEvent.Form.target##value;

  [@react.component]
  let make = () => {
    let (output, setOutput) = React.useState(() => "");

    let doParse = e => {
      setOutput(_ => e |> getValue |> parse);
    };

    <div>
      <textarea onChange=doParse />
      <br />
      <div dangerouslySetInnerHTML={"__html": output} />
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
