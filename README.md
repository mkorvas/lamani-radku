Zdá se, že ještě v roce 2025 je poptávka po programech na zalamování slov 
do řádků (anglicky "hyphenation"). Zde tedy nahrávám svůj kód napsaný pro 
zápočet z programování někdy v roce 2013. Ano, má chyby a možná zaslouží 
úplně přepsat, ale snad se z něj přece jen dá ještě něco použít.

Chcete-li, můžete mi poslat pull request a předpokládám, že ho rád přijmu.

Pro zajímavost přikládám i původní stručné zadání dotyčného zápočtového 
úkolu.

>>>
Napíšu prográmek, který dostane na vstupu český text a šířku sloupce ve znacích a na výstupu bude tentýž text s řádky nalámanými tak, aby se text vešel do sloupce o zadané šířce. Lámání řádků se bude řídit předěly mezi slabikami, jež se budou zjišťovat za využití zpravidla jasné korespondence mezi písmeny a odpovídajícími hláskami. Slabiku si přitom definuji podle Saussura, podle dodatku Fonologie v Kursu obecné lingvistiky, tj. podle tzv. uzavřenosti různých hlásek jako předěl mezi podposloupností hlásek, kde uzavřenost roste, (implozí) a tou, kde začíná opět klesat (explozí). Oproti tomu ovšem budu potřebovat provést jisté úpravy, aby se nevyskytovala dělení jako např. "vs-tup" nebo "opats-tví", jež by vážně nebyla hezká.

Celý algoritmus poběží hladově, nebude měřit ošklivost jednotlivých řádků a stejně tak se ani nebude zabývat tím, kolikrát za sebou lámal řádek vejpůl. Zato se pokusím od zasvěcených vyzvědět, jak láme řádky TeX, a porovnat svůj algoritmus (nebo spíše Saussurův) s algoritmem TeXu.
>>>
