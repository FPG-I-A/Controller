# Controller
Esse repositório serve apenas para criar um controlador para a FPGA.

Desta forma, ao invés de inserir manualmente os inputs quando testar os módulos criados, podemos utilizar um arduino due para o fazer.

Foi necessário utilizar um arduino due devido ao número de portas de I/O para comunicação paralela com a FPGA, o protocolo para essa comunicação é o envio simples dos bits de dados em ponto fixo utilizando desta forma `n_bits * n_feats` portas de comunicação sendo `n_bits` o número de bits na representação de ponto fixo e `n_feats` o número de features de entrada do módulo.

Assim, em uma aplicação com o dataset iris por exemplo, usando representação de ponto fixo Q3.5 são necessários 32 portas apenas para enviar as features, mais o pulso de init, reset e ocupado. Esse foi o caso do exemplo utilizado.

O circuito pode ser visto na pasta `controller`, trata-se de um projeto bem simples do kicad.