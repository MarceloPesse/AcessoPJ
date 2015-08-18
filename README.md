# AcessoPJ
Descrição: Firmware responsavel pela abertura automatica da porta da PJ

O microcontrolador recebe do PC (NEO) atraves da porta serial o comando de abertura
da porta, o software no PC fica responsavel por comparar o nº USP recebido do leitor
de codigo de barra com o database e mandar a mensagem coerente com a situação.

Por conveniencia, o microcontrolador possui um led bicolor que ira acender dependendo da situação.

Situação                  |  Comando  |   Cor do LED
Acesso Autorizado         |    #O     |    Verde
Acesso Negado             |    #C     |    Vermelho
Codigo de Barra Invalido  |    #I     |    Laranja              
