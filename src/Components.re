module S = {
  [@react.component]
  let make = (~children) => children |> React.string;
};
