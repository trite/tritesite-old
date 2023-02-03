// Going to try and make a parser combinator
// library using this as a bit of a rough
// guide hopefully, but using Melange instead
// of BuckleScript and Relude insted of Belt:
//   https://www.youtube.com/watch?v=OhmsTLtwLY4

type parser('a) = string => option('a);

let parse_a = (str: string) => {
  switch (str) {
  | "a" => Some("a")
  | _ => None
  };
};

parse_a("a") |> Option.fold(~none="Nope", ~some=x => {j|Yep: $x|j}) |> Js.log;
