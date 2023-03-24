let example = {j|---
title: Some title
created: 2023-03-24
---

# Some header

Some text|j};

let frontMatterDelim = [%re {|/\n?---\n?/g|}];

example
|> Js.String.match(frontMatterDelim)
|> Option.map(Array.map(Option.map(Js.String.trim)))
|> Js.log;

example
|> Js.String.splitByRe(frontMatterDelim)
|> Array.map(Option.map(Js.String.trim))
|> Js.log;
