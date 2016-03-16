Camus Tristan
Cojez Arnaud

E3
Q2.
  pour u = p1p2,
  pour I = p1 + k * u
  on cherche k tq AI.n = 0
    AI.n = 0 <=> (I - A).n = 0
    ((p1 + k * u) - A).n = 0
    (p1 + k * u - A).n = 0
    ((p1 - A) + k * u).n = 0
    (Ap1 + k * u).n = 0
    Ap1.n + (k * u).n = 0
    (k * u).n = - Ap1.n
    k* (u.n) = - Ap1.n
    k = - Ap1.n / u.n
  => formule utilisée dans intersection

Q5.
  Le rendu avec depth buffer ne dessine pas un pixel si celui-ci se trouve derrière un pixel déjà dessiné.
  Par conséquent, certains pixels ne sont ici pas dessiné. D'où les défauts dans le blending des couleurs.
  (avec le painter, tous les polygones sont dessinés => pas ce problème)

Remarque : le cours devrait un peu + détailler ces algorithmes, cette partie nous a semblé difficile.
