[@bs.module "marked"] external parse: string => string = "parse";

[@bs.module "dompurify"] external sanitize: string => string = "sanitize";

let contentRoot = "https://github.com/trite/trite.io-content";

module App = {
  let getValue = e => e->ReactEvent.Form.target##value;

  let parse = (setter, event) =>
    event |> getValue |> parse |> sanitize |> (x => setter(_ => x));

  [@react.component]
  let make = () => {
    let (output, setOutput) = React.useState(() => "");

    <div>
      <textarea onChange={parse(setOutput)} />
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
