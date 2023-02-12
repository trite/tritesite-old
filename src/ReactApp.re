[@bs.module "marked"] external parse: string => string = "parse";

[@bs.module "dompurify"] external sanitize: string => string = "sanitize";

let contentRoot = "https://api.github.com/repos/trite/trite.io-content/contents/";

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

module App = {
  let getValue = e => e->ReactEvent.Form.target##value;

  let parse = (setter, event) =>
    event |> getValue |> parse |> sanitize |> (x => setter(_ => x));

  let fetch = (setter, uri, _event) =>
    ContentFetch.fetchString(uri)
    |> IO.unsafeRunAsync(
         fun
         | Ok(content) => setter(_ => content)
         | Error(error) => setter(_ => error |> ContentFetch.Error.show),
       );

  [@react.component]
  let make = () => {
    let (parsedMarkdown, setParsedMarkdown) = React.useState(() => "");
    let (fetched, setFetched) = React.useState(() => "");

    <div>
      <textarea onChange={parse(setParsedMarkdown)} />
      <br />
      <div dangerouslySetInnerHTML={"__html": parsedMarkdown} />
      <br />
      <hr />
      <br />
      <textarea value=fetched />
      <input
        type_="button"
        value="clicky"
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
