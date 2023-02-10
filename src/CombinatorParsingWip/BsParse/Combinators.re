// open Util;

open Parser;

// (Parsers with type parse_error := ParseError.t)

module BasicCombinators: Parsers = {
  // bind equation to externally treat it like this
  type parse_error = ParseError.t;
  // type result(('a, Location.t), parse_error) = result(('a, Location.t), parse_error);
  type parser('a) =
    | ScopedParser(
        array(string),
        Location.t => result(('a, Location.t), parse_error),
      )
    | LabeledParser(
        string,
        Location.t => result(('a, Location.t), parse_error),
      )
    | Parser(Location.t => result(('a, Location.t), parse_error));

  exception CannotGet(parse_error);
  exception CannotGetError;

  // let get_exn: Util.result(('a, 'b), parse_error) => 'a =
  let get_exn: result(('a, 'b), parse_error) => 'a =
    result =>
      switch (result) {
      | Ok((res, _)) => res
      | Error(error) => raise(CannotGet(error))
      };

  let biMapResult: ('a => 'b, 'c => 'b, result(('a, 'd), 'c)) => 'b =
    (successMap, errorMap, result) =>
      switch (result) {
      | Ok((res, _loc)) => successMap(res)
      | Error(error) => errorMap(error)
      };

  let get_error: result(('a, Location.t), parse_error) => parse_error =
    result =>
      switch (result) {
      | Error(error) => error
      | _ => raise(CannotGetError)
      };

  let runParser = (p, loc) =>
    switch (p) {
    | ScopedParser(_, fn) => fn(loc)
    | LabeledParser(m, fn) =>
      switch (fn(loc)) {
      | Error(_) => Error(ParseError.make(loc, m))
      | ok => ok
      }
    | Parser(fn) => fn(loc)
    };

  let run = (parser, input) => {
    switch (runParser(parser, Location.make(input, 0))) {
    | Ok((v, loc)) => Ok((v, loc))
    | Error(err) => Error(err)
    };
  };

  let string = str =>
    Parser(
      loc => {
        open Js.String;
        let substr = loc.input->substr(~from=loc.offset);
        if (substr |> startsWith(str)) {
          let charsConsumed = str->length;
          Ok((str, loc |> Location.inc(charsConsumed)));
        } else {
          Error(ParseError.make(loc, "Expected: " ++ str));
        };
      },
    );

  let orElse = (p1, p2) =>
    Parser(
      loc => {
        switch (runParser(p1, loc)) {
        | Error(error1) =>
          switch (runParser(Lazy.force(p2), loc)) {
          | Error(error2) =>
            let stack = error2.stack;
            let otherFailures =
              Array.append([|error1|], error2.otherFailures);
            Error(ParseError.makeWith(stack, otherFailures));
          | ok => ok
          }
        | ok => ok
        }
      },
    );

  let flatMap = (p, fn) =>
    Parser(
      loc => {
        switch (runParser(p, loc)) {
        | Ok((v1, loc)) =>
          switch (runParser(fn(v1), loc)) {
          | Ok((v2, loc)) => Ok((v2, loc))
          | Error(err) => Error(err)
          }
        | Error(err) => Error(err)
        }
      },
    );

  let unit = a => Parser(loc => Ok((a, loc)));

  let listOfN = (int, p) =>
    Parser(
      loc => {
        let rec run = (i, p, loc, acc) => {
          switch (runParser(p, loc)) {
          | Ok((v, loc)) =>
            let newAcc = Array.append(acc, [|v|]);
            if (i <= 0) {
              Ok((newAcc, loc));
            } else {
              run(i - 1, p, loc, newAcc);
            };
          | Error(error) => Error(error)
          };
        };
        run(int, p, loc, [||]);
      },
    );

  let many = p =>
    Parser(
      loc => {
        let rec run = (p, loc, acc) => {
          switch (runParser(p, loc)) {
          | Ok((v, loc)) => run(p, loc, Array.append(acc, [|v|]))
          | _ => Ok((acc, loc))
          };
        };
        run(p, loc, [||]);
      },
    );

  let many1 = p =>
    Parser(
      loc => {
        switch (runParser(many(p), loc)) {
        | Ok((v, loc)) =>
          if (Array.length(v) === 0) {
            Error(
              ParseError.make(
                loc,
                "Expected at least one repetition for parser",
              ),
            );
          } else {
            Ok((v, loc));
          }
        | err => err
        }
      },
    );

  let slice = p =>
    Parser(
      loc => {
        switch (runParser(p, loc)) {
        | Ok((_, newLoc)) =>
          let charsConsumed = newLoc.offset - loc.offset;
          Ok((
            loc.input
            ->Js.String.substrAtMost(~from=loc.offset, ~length=charsConsumed),
            newLoc,
          ));
        | Error(error) => Error(error)
        }
      },
    );

  let regex = regexpr =>
    Parser(
      loc => {
        let reg = Js.Re.fromString("^" ++ regexpr);
        let sstr = Js.String.substr(~from=loc.offset, loc.input);
        if (reg->Js.Re.test_(sstr)) {
          let result =
            reg->Js.Re.exec_(sstr)->Belt.Option.getExn->Js.Re.captures
            |> Array.map(x => Js.Nullable.toOption(x) |> Belt.Option.getExn);
          let charsConsumed = result->Array.get(0)->String.length;
          // calculate the difference
          let newLoc = {...loc, offset: loc.offset + charsConsumed};
          Ok((result, newLoc));
        } else {
          Error(ParseError.make(loc, "Expected: " ++ regexpr));
        };
      },
    );

  let sepBy = (sep, p) =>
    Parser(
      loc => {
        let rec parse = (sep, p, loc, acc) => {
          switch (runParser(p, loc)) {
          | Ok((v, loc)) =>
            let newAcc = Array.append(acc, [|v|]);
            switch (runParser(sep, loc)) {
            | Ok((_, loc)) => parse(sep, p, loc, newAcc)
            | Error(_) => Ok((newAcc, loc))
            };
          | Error(_) => Ok((acc, loc))
          };
        };
        parse(sep, p, loc, [||]);
      },
    );

  let label = (m, p) =>
    switch (p) {
    | LabeledParser(_, fn) => LabeledParser(m, fn)
    | Parser(fn) => LabeledParser(m, fn)
    | a => a
    };

  let scope = (m, p) =>
    switch (p) {
    | ScopedParser(scope, fn) =>
      ScopedParser(Array.append([|m|], scope), fn)
    | LabeledParser(_, fn) => ScopedParser([|m|], fn)
    | Parser(fn) => ScopedParser([||], fn)
    };

  let attempt = p =>
    Parser(
      loc => {
        switch (runParser(p, loc)) {
        | Ok((v, loc)) => Ok((Some(v), loc))
        | _ => Ok((None, loc))
        }
      },
    );

  let fail = error => Parser(loc => {Error(ParseError.make(loc, error))});
};

module DP = DerivedParsers(BasicCombinators);
module Ops = InfixOps(BasicCombinators);

include BasicCombinators;
include DP;
include Ops;
