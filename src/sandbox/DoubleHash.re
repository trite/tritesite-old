let testThing = [%raw
  {j|{
  foo: {
    bar: {
      blah: "Well hello there!"
    }
  }
}
|j}
];

testThing |> Js.log2("all:  ");
testThing##foo |> Js.log2("foo:  ");
testThing##foo##bar |> Js.log2("bar:  ");
testThing##foo##bar##blah |> Js.log2("blah: ");

(
  switch (testThing##ohnoes |> Js.toOption) {
  | Some(_) => "exists"
  | None => "does not exist"
  }
)
|> Js.log; // does not exist

(
  switch (testThing##foo |> Js.toOption) {
  | Some(_) => "exists"
  | None => "does not exist"
  }
)
|> Js.log; // exists

testThing->Js.Dict.get("foo") |> Js.log2("Js.Dict: ");
