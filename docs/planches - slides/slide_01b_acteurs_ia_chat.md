<!-- slide: class: title-slide -->

# Acteurs Majeurs de l'IA Conversationnelle

## Interfaces chat grand public vs CLI de codage

---

<!-- slide: class: content-slide -->

## Acteurs majeurs avec interface chat

| Acteur | Interface web | Application mobile | Particularités |
|--------|---------------|-------------------|----------------|
| **OpenAI** | ChatGPT | iOS, Android | Leader, GPT-4/5, Store dGPTs |
| **Anthropic** | Claude.ai | iOS, Android | Claude 4, focus sécurité |
| **Google** | Gemini | iOS, Android | Gemini 1.5/2, intégration Workspace |
| **Meta** | Meta AI | WhatsApp, Instagram | Llama open source |
| **xAI** | Grok | X (Twitter) | Humour, accès temps réel |
| **Perplexity** | Perplexity.ai | iOS, Android | Moteur de recherche IA |
| **Mistral** | le-chat | - | Français, open source |

```mermaid
graph TD
    subgraph "Acteurs principaux"
        O[OpenAI<br/>ChatGPT]
        A[Anthropic<br/>Claude]
        G[Google<br/>Gemini]
        M[Meta<br/>Llama]
        X[xAI<br/>Grok]
        P[Perplexity]
    end
    
    O --- G
    A --- G
    M --- O
    X --- O
    P --- G
    
    style O fill:#10A37F,color:white
    style A fill:#D97757,color:white
    style G fill:#4285F4,color:white
```

---

<!-- slide: class: content-slide -->

## Chat utilisateur vs CLI de codage

```mermaid
flowchart TB
    subgraph "Chat utilisateur classique"
        U[Utilisateur] --> W[Interface web/app]
        W --> API[API cloud]
        API --> M[Modèle IA]
        M --> R[Réponse texte]
        R --> W
        W --> U
    end
    
    style U fill:#E0E0E0
    style W fill:#90CAF9
    style API fill:#64B5F6
    style M fill:#2196F3,color:white
    style R fill:#90CAF9
```

**Chat utilisateur:**
- Envoie du texte/reçois du texte
- Exécution distante (cloud)
- Pas d'accès au système de fichiers
- Pas d'exécution de commandes

---

<!-- slide: class: content-slide -->

## CLI de codage (agent)

```mermaid
flowchart TB
    subgraph "CLI Coding Agent"
        U[Utilisateur] --> CLI[Terminal<br/>CLI]
        CLI --> AGENT[Agent IA]
        AGENT --> |Shell| SH[Commandes<br/>système]
        AGENT --> |File I/O| FS[Système de<br/>fichiers]
        AGENT --> |Git| GIT[Version<br/>control]
        SH --> |stdout/stderr| AGENT
        FS --> AGENT
        GIT --> AGENT
        AGENT --> CLI
        CLI --> U
    end
    
    style U fill:#E0E0E0
    style CLI fill:#FFA726
    style AGENT fill:#EF6C00,color:white
    style SH fill:#FFCC80
    style FS fill:#FFCC80
    style GIT fill:#FFCC80
```

**CLI de codage:**
- Exécute des commandes shell
- Lit/écrit des fichiers localement
- Gère Git (commit, push, etc.)
- Accès complet à la machine hôte

---

<!-- slide: class: content-slide -->

## Différences clés

| Caractéristique | Chat classique | CLI de codage |
|-----------------|----------------|---------------|
| **Environnement** | Cloud (distance) | Local (machine hôte) |
| **Actions** | Texte uniquement | Shell, fichiers, git |
| **Sécurité** | Limité par l'API | Accès root possible |
| **Use case** | Q&R, brainstorming | Développement, debugging |
| **Exemples** | ChatGPT, Claude.ai | Claude Code, OpenCode, Copilot CLI |

```mermaid
graph LR
    subgraph "Flux d'information"
        A[Prompt] --> B{Type d'interface?}
        B -->|Chat| C[API Cloud<br/>→ Réponse texte]
        B -->|CLI| D[Agent local<br/>→ Actions système]
    end
    
    style A fill:#E0E0E0
    style B fill:#FF9800,color:white
    style C fill:#4CAF50,color:white
    style D fill:#F44336,color:white
```
