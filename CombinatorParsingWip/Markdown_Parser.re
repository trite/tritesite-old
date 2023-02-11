// https://www.markdownguide.org/cheat-sheet/

let (>>) = Relude.Globals.(>>);

module MarkdownTypes = {
  type heading =
    | H1(string)
    | H2(string)
    | H3(string);

  type link = {
    title: string,
    uri: string,
  };

  type image = {
    altText: string,
    path: string,
  };

  type text =
    | Plain(string)
    | Bold(string)
    | Italic(string)
    | BoldAndItalic(string)
    | Strikethrough(string);
  // | Subscript // TODO: add later
  // | Superscript // TODO: add later

  type element =
    | Heading(heading)
    | Text(array(text))
    | Blockquote(string)
    | OrderedList(list(string))
    | UnorderedList(list(string))
    | Code(string)
    | HorizontalRule
    | Link(link)
    | Image(image);
};

module Parsing = {
  open Combinators;
  open MarkdownTypes;
  open CommonCombinators;

  let const = (x, _) => x;

  let contentTillLineEnd = regex("[^\r\n]*") |> slice;

  let whitespaceTillLineEnd = whitespace >>= const(contentTillLineEnd);

  let heading1 =
    string("#")
    >>= (
      _ => whitespace >>= (_ => contentTillLineEnd <$> (s => Heading(H1(s))))
    );

  let heading2 =
    string("##")
    >>= (
      _ => whitespace >>= (_ => contentTillLineEnd <$> (s => Heading(H2(s))))
    );

  let heading3 =
    string("###")
    >>= (
      _ => whitespace >>= (_ => contentTillLineEnd <$> (s => Heading(H3(s))))
    );

  let headingAny = heading3 <|> (lazy(heading2)) <|> (lazy(heading1));

  // Markdown presents a weird problem for parsing:
  //    How does one know when to end regular text?
  //    Some elements are easy to parse when they end:
  //      **some bold text** just looks for text between bold tags (**)
  //    But for elements not wrapped in identifiers what's the solution?
  //      I'm thinking just let lines like that parse individually
  //      Then do a pass over the AST to find sequential "joinable"
  //        elements and "join" them

  // IMPORTANT: Plain text parser will need to be prioritized after other text parsers
  let plainText = contentTillLineEnd |> slice;

  let boldText = surround(string("**"), regex(".*\*\*"), string("**"));

  type expressionResult =
    | Element(element)
    | Ignore(string)
    | Eof;

  let anyElement = headingAny <$> (e => Element(e));

  let ignored =
    endLine <|> (lazy(whitespaceTillLineEnd)) <$> (_ => Ignore("whitespace"));

  let endOfFile = eof <$> (_ => Eof);

  let expressions = endOfFile <|> (lazy(anyElement)) <|> (lazy(ignored));

  let rec run = (loc: Location.t, acc: array(expressionResult)) =>
    Combinators.runParser(expressions, loc)
    |> (
      fun
      | Ok((Element(e), loc)) =>
        run(loc, Array.append(acc, [|Element(e)|]))
      | Ok((Ignore(s), loc)) => run(loc, Array.append(acc, [|Ignore(s)|]))
      | Ok((Eof, loc)) => Ok((Array.append(acc, [|Eof|]), loc))
      | _ => Ok((acc, loc))
    );

  let run = str => run(Location.make(str, 0), [||]);
};

let textToString =
  MarkdownTypes.(
    fun
    | Plain(s) => s
    | Bold(s) => {j|<strong>$s</strong>|j}
    | Italic(s) => {j|<em>$s</em>|j}
    | BoldAndItalic(s) => {j|<strong><em>$s</em></strong>|j}
    | Strikethrough(s) => {j|<del>$s</del>|j}
  );

let textsToString =
  Array.map(textToString) >> Relude.Array.String.joinWith(" ");

let elemToString =
  MarkdownTypes.(
    fun
    | Heading(H1(heading)) => {j|heading 1: $heading|j}
    | Heading(H2(heading)) => {j|heading 2: $heading|j}
    | Heading(H3(heading)) => {j|heading 3: $heading|j}
    | Text(texts) => texts |> textsToString |> (x => {j|text: $x|j})
    | Blockquote(string) => {j|blockquote: $string|j}
    | OrderedList(_) => {j|ol: blah|j}
    | UnorderedList(_) => {j|ul: blah|j}
    | Code(string) => {j|code: $string|j}
    | HorizontalRule => {j|hr|j}
    | Link(link) => {j|link: $link|j}
    | Image(image) => {j|image: $image|j}
  );

let expressionToString =
  Parsing.(
    fun
    | Element(e) => elemToString(e)
    | Ignore(s) => {j|-- IGNORE ($s) --|j}
    | Eof => "-- EOF --"
  );

let resultToString = result =>
  switch (result) {
  | Ok((resultArray, _)) =>
    resultArray
    // |> Array.map(elemToString)
    |> Array.map(expressionToString)
    |> Relude_Array.String.joinWith("\n")
  | Error(error) => error |> ParseError.toString
  };

// "# Blah blah blah this is some title"
// |> Combinators.run(Parsers.expr)
// |> resultToString
// |> Js.log;

{j|# Blah blah blah this is some title with header1
## Blah header2
### Oh look a header3!
|j}
// |> Combinators.run(Parsing.run)
// "# Blah blah blah this is some title"
|> Parsing.run
|> resultToString
|> Js.log;
