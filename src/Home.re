module Styles = {
  open Css;

  let wideTextInput = style([width(pct(50.0))]);

  let wideTextArea = style([width(pct(50.0)), height(px(150))]);
};

[@react.component]
let make = () => {
  module S = Components.S;

  <div> <S> "blah" </S> </div>;
};
