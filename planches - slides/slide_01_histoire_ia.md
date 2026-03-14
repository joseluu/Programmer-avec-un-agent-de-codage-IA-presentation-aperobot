<!-- slide: class: title-slide -->

# Histoire de l'IA Moderne

## De la reconnaissance d'image aux fondationaux des LLMs

---

<!-- slide: class: content-slide -->

## Contexte: L'émergence du deep learning

- **2012**: AlexNet bat les algorithmes classiques sur ImageNet
  - Taux d'erreur: ~15% vs ~25% pour les méthodes traditionnelles
  - Première démonstration à grande échelle de la supériorité des réseaux de neurones profonds

- **2015**: ResNet atteint une erreur < 3.5% - performance surhumaine
  - Plus de 150 couches de profondeur
  - Introduction des connexions résiduelles

```mermaid
graph LR
    A[Images] --> B[Feature Extraction]
    B --> C[Réseau de neurones profond]
    C --> D[Classification]
    D --> E[Label prédit]
    
    style C fill:#4CAF50,color:white
```

---

<!-- slide: class: content-slide -->

## Limites et nouveau défi: le texte

- Les images sont des données continues, le texte est **discret**
- Problème: comment représenter des mots pour un réseau de neurones?

- **Solution: La tokenisation**
  - Découper le texte en unités atomiques (mots, sous-mots, caractères)
  - Chaque token devient un vecteur numérique

```mermaid
graph LR
    A["Le chat mange"] --> B[Tokenisation]
    B --> C["[Le]  [chat] [mange]"]
    C --> D[Embedding]
    D --> E["[0.1, -0.3, ...] [0.5, 0.2, ...] [-0.2, 0.8, ...]"]
    
    style B fill:#2196F3,color:white
    style D fill:#2196F3,color:white
```

---

<!-- slide: class: content-slide -->

## Séquence et génération de texte

- Le texte est intrinsèquement **séquentiel**
- Idée clé: prédire le prochain token à partir des précédents
- Architecture séquence-à-séquence (Seq2Seq)

```mermaid
flowchart LR
    subgraph Entrée
        T1["Token t-2"]
        T2["Token t-1"]
    end
    
    subgraph Modèle
        RNN[Réseau Récurrent<br/>ou Transformer]
    end
    
    subgraph Sortie
        T3["Token t"]
        T4["Token t+1"]
    end
    
    T1 --> RNN
    T2 --> RNN
    RNN --> T3
    T3 --> T4
    
    style RNN fill:#9C27B0,color:white
```

---

<!-- slide: class: content-slide -->

## Évolution des architectures

| Époque | Architecture | Usage principal |
|--------|--------------|-----------------|
| 2013-2016 | Word2Vec, GloVe | Embeddings statiques |
| 2014-2017 | LSTM, GRU | Traduction, seq2seq |
| 2017+ | **Transformer** | Tout type de tâche NLP |
| 2018+ | GPT, BERT | Génération & compréhension |

```mermaid
timeline
    title Chronologie des architectures
    2013 : Word2Vec
    2014 : Seq2Seq + Attention
    2017 : Transformer (Attention is All You Need)
    2018 : GPT-1, BERT
    2019 : GPT-2, T5
    2020 : GPT-3
    2022 : ChatGPT
    2023 : GPT-4, LLaMA, Claude
```
