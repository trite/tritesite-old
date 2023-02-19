type links = {
  self: string,
  git: string,
  html: string,
};

type githubApiResponse = {
  name: string,
  path: string,
  sha: string,
  size: int,
  url: string,
  html_url: string,
  git_url: string,
  download_url: string,
  type_: string,
  content: string,
  content_decoded: string,
  encoding: string,
  _links: links,
};

let githubApiResponseToString =
    (
      {
        name,
        path,
        sha,
        size,
        url,
        html_url,
        git_url,
        download_url,
        type_,
        content,
        content_decoded,
        encoding,
        _links: {self, git, html},
      },
    ) => {j|
  name:            $name
  path:            $path
  sha:             $sha
  size:            $size
  url:             $url
  html_url:        $html_url
  git_url:         $git_url
  download_url:    $download_url
  type:            $type_
  content:         $content
  content_decode:  $content_decoded
  encoding:        $encoding
  ----links----
    self: $self
    git:  $git
    html: $html
|j};

let linksMake = (self, git, html) => {self, git, html};

let githubApiMake =
    (
      name,
      path,
      sha,
      size,
      url,
      html_url,
      git_url,
      download_url,
      type_,
      content,
      encoding,
      _links,
    ) => {
  name,
  path,
  sha,
  size,
  url,
  html_url,
  git_url,
  download_url,
  type_,
  content: content |> String.replaceEach(~search="\n", ~replaceWith=""),
  content_decoded: content |> Base64.decode,
  encoding,
  _links,
};

let decodeLinks = (json): result(links, Decode_ParseError.failure) =>
  Decode.AsResult.OfParseError.Pipeline.(
    succeed(linksMake)
    |> field("self", string)
    |> field("git", string)
    |> field("html", string)
    |> run(json)
  );

let decodeTest = json =>
  Decode.AsResult.OfParseError.Pipeline.(
    succeed(githubApiMake)
    |> field("name", string)
    |> field("path", string)
    |> field("sha", string)
    |> field("size", intFromNumber)
    |> field("url", string)
    |> field("html_url", string)
    |> field("git_url", string)
    |> field("download_url", string)
    |> field("type", string)
    |> field("content", string)
    |> field("encoding", string)
    |> field("_links", decodeLinks)
    |> run(json)
  );
