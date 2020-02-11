### Lightweight Directory Access Protocol (LDAP) server

- `myldap [-p <port>] -f <file>`
  - example: `./myldap -p 3899 -f data/isa2017-ldap.csv`

- missing: substrings (4)

- testing query from ldap client:
  - `ldapsearch -vu -H ldap://localhost:3899 -x+ "(|(uid=xlukac09)(uid=xlogin00)(&(!(uid=xam...))(uid=xan...)))"`
