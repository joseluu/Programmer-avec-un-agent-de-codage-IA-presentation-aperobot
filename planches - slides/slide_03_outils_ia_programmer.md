<!-- slide: class: title-slide -->

# Les Outils IA pour Programmer

## IDEs et CLI avec IA intégrée

---

<!-- slide: class: content-slide -->

## IDEs avec Intelligence Artificielle

### Solutions intégrées (fork de VS Code)

| IDE | Prix | Particularités |
|-----|------|----------------|
| **Cursor** | $20/mois (Pro) | Leader du marché, ~77% SWE-bench, 12M utilisateurs |
| **Windsurf** (Codeium) | $15/mois | Acquis par Cognition (Devin AI), agent Cascade |
| **Google Antigravity** | Gratuit (preview) | 5 agents parallèles, 76.2% SWE-bench |
| **Zed** | Gratuit | AI intégré, éditeur ultra-rapide |

### IDEs classiques avec extensions

| IDE | Extension IA | Prix |
|-----|--------------|------|
| **VS Code** | GitHub Copilot | Gratuit / $10/mois |
| **JetBrains** | AI Assistant | Inclus dans l'abonnement |
| **Trae AI** | Web-based | Gratuit (Claude 3.7) |

```mermaid
graph TD
    subgraph "AI IDEs"
        C[Cursor]
        W[Windsurf]
        A[Antigravity]
        Z[Zed]
    end
    
    subgraph "Extensions"
        VC[VS Code + Copilot]
        JB[JetBrains AI]
    end
    
    C --> P[Payant]
    W --> P
    A --> G[Gratuit]
    Z --> G
    VC --> M[Mixed]
    JB --> P
    
    style C fill:#6366F1,color:white
    style A fill:#34D399,color:black
    style Z fill:#10B981,color:white
```

---

<!-- slide: class: content-slide -->

## CLI pour programmer avec IA

### Outils en ligne de commande

```mermaid
flowchart LR
    subgraph "CLI IA"
        CC[Claude Code]
        CP[Copilot CLI]
        OC[OpenCode]
        GC[Gemini CLI]
        AR[Aider]
    end
    
    CC --> M1[Anthropic]
    CP --> M2[OpenAI/Google/Anthropic]
    OC --> M3[Any LLM]
    GC --> M4[Google]
    AR --> M5[Open Source]
```

---

<!-- slide: class: content-slide -->

## CLI: Modèles disponibles

### **Claude Code** (Anthropic)

| Type | Modèles |
|------|---------|
| **Gratuit** | Qwen3-Coder, DeepSeek (via Ollama), Grok Code (Together AI) |
| **Payant** | Claude 3.5 Sonnet, Claude 3.7 Sonnet, Claude 4 Haiku/Sonnet/Opus |

### **GitHub Copilot CLI**

| Type | Modèles |
|------|---------|
| **Gratuit** | GPT-5 mini, GPT-4.1, Grok Code Fast 1, Haiku 4.5 (0x credits) |
| **Payant** | Claude Sonnet 4.6, Claude Opus 4.6, GPT-5.3/5.4 Codex, Gemini 3 Pro |

### **OpenCode**

| Type | Modèles |
|------|---------|
| **Gratuit** | Via Copilot/ChatGPT (compte existant), 75+ providers (API) |
| **Payant** | Zen (service modèle optimisé) |

### **Gemini CLI** (Google)

| Type | Modèles |
|------|---------|
| **Gratuit** | Gemini Flash, Gemini Pro (Google AI Studio) |
| **Payant** | Gemini Ultra |

### **Aider**

| Type | Modèles |
|------|---------|
| **Gratuit** | Qwen3-Coder, DeepSeek-Coder, CodeLlama (via Ollama) |
| **Payant** | API nécessaire pour autres modèles |

---

<!-- slide: class: content-slide -->

## Comparaison resumée

```mermaid
quadrantChart
    title Positionnement des outils CLI
    x-axis Faible --> Forte
    y-axis Gratuit --> Payant
    quadrant-1 Leader
    quadrant-2 Alternatives
    quadrant-3 Experimental
    quadrant-4 Usage specifique
    
    "Claude Code": [0.7, 0.6]
    "Copilot CLI": [0.6, 0.4]
    "OpenCode": [0.5, 0.3]
    "Gemini CLI": [0.4, 0.2]
    "Aider": [0.3, 0.1]
```

### Points clés

- **Claude Code**: Meilleure expérience agent, flexible (Ollama = gratuit)
- **Copilot CLI**: Intégration GitHub, gratuit avec Copilot Free
- **OpenCode**: Open source, connexion ANY modèle (75+ providers)
- **Gemini CLI**: Gratuit via Google AI Studio
- **Aider**: Minimaliste, orienté pair programming

