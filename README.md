# alocs
API desenvolvida para explorar a alocação de dados com controle sobre a localidade, com o objetivo de
gerenciar a alocação de dados em sistemas de armazenamento distribuído, promovendo a comunicação
entre a aplicação, os sistemas de armazenamento e controle de metadados, permitindo a alocação de um conjunto de
pares chave-valor agrupados em uma única estrutura, cuja localidade é controlada pela aplicação.

A API é formada por 4 arquivos:
common.c concentra as funções, e tipos comuns a toda a API
alocs.c funções relativas a interface de aplicação
iceph.c funções relativas a interface do sistema de armazenamento
ceph_lib.c funções para comunicação com a API do sistema de armazenamento
