# Transcrição de Texto em Áudio Estéreo com Base nas Frequências de Guido d’Arezzo

Este projeto em C converte texto em áudio estéreo utilizando princípios musicais inspirados nas frequências das notas derivadas do sistema de Guido d’Arezzo.  
Cada caractere do texto (vogais, consoantes e espaços) é mapeado para frequências específicas ou pausas, gerando um “desenho sonoro” do texto.

A ideia central é transformar texto em uma sequência de sons, onde:
- Vogais controlam principalmente o canal **esquerdo**;
- Consoantes controlam principalmente o canal **direito**;
- Espaços geram **silêncio**;
- Outros caracteres mantêm a paisagem sonora atual.

---

## Conceito

O projeto se baseia em algumas ideias principais:

- Aqui, usamos um conjunto de frequências que remetem às notas musicais (por exemplo, próximas a dó, ré, mi, fá, sol, lá, si), associando-as às letras do texto.

- **Texto → Som**:
  - **Vogais (`a, e, i, o, u`...)**  
    Mapeadas para frequências do **canal esquerdo** (`nota_esq`), criando uma linha melódica base.
  - **Consoantes (`b, c, d, f`...)**  
    Mapeadas para frequências do **canal direito** (`nota_dir`), gerando uma espécie de contraponto rítmico/harmônico.
  - **Espaços (`' '`)**  
    Convertidos em **silêncio** (pausas) em ambos os canais.
  - **Outros caracteres**  
    Não alteram as frequências, mas continuam gerando som com as notas atuais.
