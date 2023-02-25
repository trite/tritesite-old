// Why on earth were these names chosen for this behavior?
// Looks like I'm not the only one who this has confused:
//   https://stackoverflow.com/questions/33854103/why-were-javascript-atob-and-btoa-named-like-that

[@genType.import "./"] external btoa: string => string = "btoa";
let encode = btoa;

external atob: string => string = "atob";
let decode = atob;
