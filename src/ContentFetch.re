module Error = {
  type t = ReludeFetch.Error.t(string);
  let show = error => ReludeFetch.Error.show(a => a, error);
  module Type = {
    type nonrec t = t;
  };
};

module IOE = IO.WithError(Error.Type);
open IOE.Infix;

let fetchString = uri => ReludeFetch.fetch(uri) >>= ReludeFetch.Response.text;
