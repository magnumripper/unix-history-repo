<!--
     The FreeBSD Documentation Project
     The FreeBSD French Documentation Project

     $Id$
     $FreeBSD$
     Original revision: 1.4
-->

<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY release.dsl PUBLIC "-//FreeBSD//DOCUMENT Release Notes DocBook Language Neutral Stylesheet//EN" CDATA DSSSL>
<!ENTITY % output.html  "IGNORE"> 
<!ENTITY % output.print "IGNORE">
]>

<style-sheet>
  <style-specification use="docbook">
    <style-specification-body>
 
      <![ %output.html; [ 
	(define ($email-footer$)
          (make sequence
	    (make element gi: "p"
                  attributes: (list (list "align" "center"))
              (make element gi: "small"
                (literal "Ce fichier, et les autres documents concernant cette version sont t�l�chargeables sur ")
		(create-link (list (list "HREF" (entity-text "release.url")))
                  (literal (entity-text "release.url")))
                (literal ".")))
            (make element gi: "p"
                  attributes: (list (list "align" "center"))
              (make element gi: "small"  
                (literal "Pour les questions sur FreeBSD, lisez la ")
		(create-link
		  (list (list "HREF" "http://www.FreeBSD.org/docs.html"))
                  (literal "documentation"))
                (literal " avant de contacter <")
		(create-link
		  (list (list "HREF" "mailto:questions@FreeBSD.org"))
                  (literal "questions@FreeBSD.org"))
                (literal ">.")
            (make element gi: "p"
                  attributes: (list (list "align" "center"))
              (make element gi: "small"  
                (literal "Tout utilisateur de FreeBSD ")
		(literal (entity-text "release.branch"))
		(literal " doit souscrire � la liste �lectronique")
                (literal "<")
		(create-link (list (list "HREF" "mailto:stable@FreeBSD.org"))
                  (literal "stable@FreeBSD.org"))
                ))

            (make element gi: "p"
                  attributes: (list (list "align" "center"))
	      (literal "Pour les questions sur ce document, contactez par mail <")
	      (create-link (list (list "HREF" "mailto:doc@FreeBSD.org"))
                (literal "doc@FreeBSD.org"))
	      (literal ">."))))))

	<!-- Convert " ... " to `` ... '' in the HTML output. -->
	(element quote
	  (make sequence
	    (literal "``")
	    (process-children)
	    (literal "''")))

        <!-- Generate links to HTML man pages -->
        (define %refentry-xref-link% #t)

        <!-- Specify how to generate the man page link HREF -->
        (define ($create-refentry-xref-link$ #!optional (n (current-node)))
          (let* ((r (select-elements (children n) (normalize "refentrytitle")))
                 (m (select-elements (children n) (normalize "manvolnum"))))
	  (string-append "http://www.FreeBSD.org/cgi/man.cgi?query="
			 (data r) "&" "sektion=" (data m)
			 "&" "manpath=FreeBSD+5.0-current")))
      ]]>

      (define (toc-depth nd)
        (if (string=? (gi nd) (normalize "book"))
            3
            3))

    </style-specification-body>
  </style-specification>

  <external-specification id="docbook" document="release.dsl">
</style-sheet>
