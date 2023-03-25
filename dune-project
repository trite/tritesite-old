(lang dune 3.8)

(using melange 0.1)

; (generate_opam_files true)

(name tritesite)

(license "MIT")

(maintainers <your_name>)

(authors <your_name>)

(homepage https://github.com/trite/tritesite)

(bug_reports https://github.com/trite/tritesite/issues)

(package
 (allow_empty)
 (name tritesite)
 (synopsis "Melange project")
 (description "Minimal melange project"))
 ; (depends
 ;  (ocaml
 ;   (and
 ;    (>= "4.14.0")
 ;    (< "4.15.0")))
 ;  (reason
 ;   (and
 ;    (>= "3.8.2")
 ;    (< "4.0.0")))
 ;  mel
 ;  melange
 ;  (ocaml-lsp-server :dev)
 ;  (dot-merlin-reader :dev)))
