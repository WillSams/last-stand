# Last Stand — Itch.io Page Theme Settings

## Game Identity
- **Title**: Last Stand
- **Tagline**: Kite the horde. Shoot what closes in. Trade retreat for time.
- **Genre**: Top-down arena survival / Twin-stick shooter
- **Engine**: Storm! Engine v2 (C++17, SDL2)

---

## Color Palette (Dark SaaS Style — Adapted for Itch.io)

| Role | Hex | Usage |
|------|-----|-------|
| **Background Deep** | `#030305` | Page background, hero section |
| **Background Elevated** | `#0a0a0f` | Cards, panels, content sections |
| **Surface** | `#14141a` | Input fields, buttons (default), modals |
| **Border Subtle** | `#1e1e2a` | Card borders, dividers |
| **Border Accent** | `#3b3b5c` | Focus states, hover borders |
| **Primary Accent** | `#8b5cf6` | Primary CTAs, links, highlights (violet-500) |
| **Primary Accent Hover** | `#7c3aed` | Button hover, link hover (violet-600) |
| **Primary Accent Muted** | `#8b5cf633` | Badge backgrounds, glows (violet-500/20) |
| **Secondary Accent** | `#ec4899` | Secondary actions, weapon pickups (pink-500) |
| **Danger** | `#ef4444` | Health warnings, death screen (red-500) |
| **Health Green** | `#22c55e` | Health regen, pickups (green-500) |
| **Gold** | `#fbbf24` | Score, high scores (amber-400) |
| **Text Primary** | `#f8fafc` | Headings, body text (slate-50) |
| **Text Secondary** | `#94a3b8` | Descriptions, meta text (slate-400) |
| **Text Muted** | `#64748b` | Timestamps, footnotes (slate-500) |

---

## Itch.io Theme CSS Variables

Paste into **Game Page → Edit Theme → Custom CSS**:

```css
:root {
  /* === Core Palette === */
  --bg-deep: #030305;
  --bg-elevated: #0a0a0f;
  --bg-surface: #14141a;
  --border-subtle: #1e1e2a;
  --border-accent: #3b3b5c;
  --accent-primary: #8b5cf6;
  --accent-primary-hover: #7c3aed;
  --accent-primary-muted: #8b5cf633;
  --accent-secondary: #ec4899;
  --danger: #ef4444;
  --health: #22c55e;
  --gold: #fbbf24;
  --text-primary: #f8fafc;
  --text-secondary: #94a3b8;
  --text-muted: #64748b;

  /* === Typography === */
  --font-heading: 'Rajdhani', 'Orbitron', sans-serif;
  --font-body: 'IBM Plex Mono', 'JetBrains Mono', monospace;
  --font-ui: 'Inter', system-ui, sans-serif;

  /* === Spacing Scale === */
  --space-xs: 4px;
  --space-sm: 8px;
  --space-md: 16px;
  --space-lg: 24px;
  --space-xl: 32px;
  --space-2xl: 48px;
  --space-3xl: 64px;

  /* === Border Radius === */
  --radius-sm: 4px;
  --radius-md: 8px;
  --radius-lg: 12px;
  --radius-xl: 16px;
  --radius-full: 9999px;

  /* === Shadows === */
  --shadow-sm: 0 1px 2px rgba(0,0,0,0.5);
  --shadow-md: 0 4px 12px rgba(0,0,0,0.6);
  --shadow-lg: 0 8px 24px rgba(0,0,0,0.7);
  --shadow-glow: 0 0 24px var(--accent-primary-muted);
  --shadow-glow-strong: 0 0 48px var(--accent-primary-muted);

  /* === Transitions === */
  --transition-fast: 120ms ease-out;
  --transition-base: 200ms ease-out;
  --transition-slow: 300ms ease-out;
}

/* === Global Page Overrides === */
body,
.game_page,
.game_page * {
  background-color: var(--bg-deep) !important;
  color: var(--text-primary) !important;
  font-family: var(--font-ui) !important;
}

/* Header / Nav Bar */
.game_header,
.game_header * {
  background: var(--bg-elevated) !important;
  border-bottom: 1px solid var(--border-subtle) !important;
}
.game_header a,
.game_header .game_title {
  color: var(--text-primary) !important;
}

/* Main Content Area */
.game_main,
.game_content,
.formatted_content {
  background: var(--bg-deep) !important;
  max-width: 960px !important;
  margin: 0 auto !important;
  padding: var(--space-xl) var(--space-lg) !important;
}

/* === Headings === */
h1, h2, h3, h4, h5, h6,
.formatted_content h1,
.formatted_content h2,
.formatted_content h3 {
  font-family: var(--font-heading) !important;
  color: var(--text-primary) !important;
  font-weight: 700 !important;
  letter-spacing: -0.02em !important;
}
h1, .formatted_content h1 {
  font-size: clamp(2rem, 5vw, 3.5rem) !important;
  background: linear-gradient(135deg, var(--accent-primary) 0%, var(--accent-secondary) 100%) !important;
  -webkit-background-clip: text !important;
  -webkit-text-fill-color: transparent !important;
  background-clip: text !important;
  margin-bottom: var(--space-md) !important;
}
h2, .formatted_content h2 {
  font-size: clamp(1.5rem, 3vw, 2rem) !important;
  border-left: 3px solid var(--accent-primary) !important;
  padding-left: var(--space-md) !important;
  margin: var(--space-2xl) 0 var(--space-md) !important;
}
h3, .formatted_content h3 {
  font-size: clamp(1.125rem, 2vw, 1.375rem) !important;
  color: var(--text-secondary) !important;
  margin: var(--space-xl) 0 var(--space-sm) !important;
}

/* === Body Text === */
p, li, .formatted_content p {
  font-family: var(--font-body) !important;
  color: var(--text-secondary) !important;
  line-height: 1.7 !important;
  font-size: 1rem !important;
  margin-bottom: var(--space-md) !important;
}

/* === Links === */
a, .formatted_content a {
  color: var(--accent-primary) !important;
  text-decoration: none !important;
  border-bottom: 1px solid transparent !important;
  transition: all var(--transition-fast) !important;
}
a:hover, .formatted_content a:hover {
  color: var(--accent-primary-hover) !important;
  border-bottom-color: var(--accent-primary-hover) !important;
}

/* === Buttons (Download, Buy, Follow) === */
.buy_btn,
.download_btn,
.follow_btn,
.game_action_btn,
button.game_action_btn {
  font-family: var(--font-ui) !important;
  font-weight: 600 !important;
  border-radius: var(--radius-md) !important;
  padding: var(--space-sm) var(--space-lg) !important;
  transition: all var(--transition-fast) !important;
  border: none !important;
  cursor: pointer !important;
}

/* Primary CTA */
.buy_btn,
.game_action_btn.primary {
  background: var(--accent-primary) !important;
  color: white !important;
  box-shadow: var(--shadow-glow) !important;
}
.buy_btn:hover,
.game_action_btn.primary:hover {
  background: var(--accent-primary-hover) !important;
  box-shadow: var(--shadow-glow-strong) !important;
  transform: translateY(-1px) !important;
}

/* Secondary / Ghost */
.download_btn,
.game_action_btn.secondary {
  background: transparent !important;
  color: var(--text-primary) !important;
  border: 1px solid var(--border-accent) !important;
}
.download_btn:hover,
.game_action_btn.secondary:hover {
  background: var(--bg-surface) !important;
  border-color: var(--accent-primary) !important;
  color: var(--accent-primary) !important;
}

/* Follow Button */
.follow_btn {
  background: var(--bg-surface) !important;
  color: var(--text-secondary) !important;
  border: 1px solid var(--border-subtle) !important;
}
.follow_btn:hover {
  background: var(--accent-primary-muted) !important;
  border-color: var(--accent-primary) !important;
  color: var(--accent-primary) !important;
}

/* === Game Info Panel (Sidebar) === */
.game_info_panel,
.game_classification,
.game_meta {
  background: var(--bg-elevated) !important;
  border: 1px solid var(--border-subtle) !important;
  border-radius: var(--radius-lg) !important;
  padding: var(--space-lg) !important;
}
.game_info_panel .label,
.game_classification .label {
  color: var(--text-muted) !important;
  font-size: 0.75rem !important;
  text-transform: uppercase !important;
  letter-spacing: 0.05em !important;
  font-family: var(--font-ui) !important;
}
.game_info_panel .value,
.game_classification .value {
  color: var(--text-primary) !important;
  font-family: var(--font-body) !important;
}

/* === Screenshot Gallery === */
.screenshot_list,
.screenshots,
.gallery {
  background: transparent !important;
}
.screenshot_item img,
.screenshots img {
  border-radius: var(--radius-lg) !important;
  border: 1px solid var(--border-subtle) !important;
  transition: all var(--transition-base) !important;
}
.screenshot_item img:hover,
.screenshots img:hover {
  border-color: var(--accent-primary) !important;
  box-shadow: var(--shadow-glow) !important;
  transform: scale(1.01) !important;
}

/* === Video Embed === */
.video_wrapper,
.embed_container {
  border-radius: var(--radius-lg) !important;
  overflow: hidden !important;
  border: 1px solid var(--border-subtle) !important;
}
.video_wrapper:hover,
.embed_container:hover {
  border-color: var(--accent-primary) !important;
  box-shadow: var(--shadow-glow) !important;
}

/* === Tags / Categories === */
.game_tags,
.tags_list,
.tag {
  background: var(--bg-surface) !important;
  color: var(--text-secondary) !important;
  border: 1px solid var(--border-subtle) !important;
  border-radius: var(--radius-full) !important;
  padding: var(--space-xs) var(--space-sm) !important;
  font-size: 0.75rem !important;
  font-family: var(--font-ui) !important;
  transition: all var(--transition-fast) !important;
}
.game_tags a:hover,
.tag:hover {
  background: var(--accent-primary-muted) !important;
  border-color: var(--accent-primary) !important;
  color: var(--accent-primary) !important;
}

/* === Comments Section === */
.comments_section,
.comment_list,
.comment {
  background: var(--bg-elevated) !important;
  border-color: var(--border-subtle) !important;
}
.comment_form textarea,
.comment_form input {
  background: var(--bg-surface) !important;
  border: 1px solid var(--border-accent) !important;
  color: var(--text-primary) !important;
  border-radius: var(--radius-md) !important;
  font-family: var(--font-body) !important;
}
.comment_form textarea:focus,
.comment_form input:focus {
  border-color: var(--accent-primary) !important;
  box-shadow: 0 0 0 3px var(--accent-primary-muted) !important;
  outline: none !important;
}

/* === Footer === */
.game_footer,
.footer {
  background: var(--bg-elevated) !important;
  border-top: 1px solid var(--border-subtle) !important;
  color: var(--text-muted) !important;
}

/* === Scrollbar === */
::-webkit-scrollbar {
  width: 8px;
  height: 8px;
}
::-webkit-scrollbar-track {
  background: var(--bg-deep);
}
::-webkit-scrollbar-thumb {
  background: var(--border-accent);
  border-radius: var(--radius-full);
}
::-webkit-scrollbar-thumb:hover {
  background: var(--accent-primary);
}

/* === Selection === */
::selection {
  background: var(--accent-primary-muted);
  color: var(--text-primary);
}

/* === Focus Visible === */
:focus-visible {
  outline: 2px solid var(--accent-primary) !important;
  outline-offset: 2px !important;
}

/* === Utility Classes for Description Markdown === */
.stat-box {
  background: var(--bg-elevated);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-lg);
  padding: var(--space-lg);
  text-align: center;
  transition: all var(--transition-base);
}
.stat-box:hover {
  border-color: var(--accent-primary);
  box-shadow: var(--shadow-glow);
}
.stat-value {
  font-family: var(--font-heading);
  font-size: 2.5rem;
  font-weight: 700;
  background: linear-gradient(135deg, var(--accent-primary), var(--accent-secondary));
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
}
.stat-label {
  font-size: 0.75rem;
  text-transform: uppercase;
  letter-spacing: 0.05em;
  color: var(--text-muted);
  margin-top: var(--space-xs);
}

.feature-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
  gap: var(--space-md);
  margin: var(--space-xl) 0;
}
.feature-card {
  background: var(--bg-elevated);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-lg);
  padding: var(--space-lg);
  transition: all var(--transition-base);
}
.feature-card:hover {
  border-color: var(--accent-primary);
  box-shadow: var(--shadow-md);
}
.feature-icon {
  width: 48px;
  height: 48px;
  background: var(--accent-primary-muted);
  border-radius: var(--radius-md);
  display: flex;
  align-items: center;
  justify-content: center;
  margin-bottom: var(--space-sm);
  color: var(--accent-primary);
}
.feature-title {
  font-family: var(--font-heading);
  font-size: 1.125rem;
  color: var(--text-primary);
  margin-bottom: var(--space-xs);
}
.feature-desc {
  color: var(--text-secondary);
  font-size: 0.875rem;
  margin: 0;
}

.weapon-card {
  background: var(--bg-elevated);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-lg);
  padding: var(--space-md);
  text-align: center;
  transition: all var(--transition-base);
}
.weapon-card:hover {
  border-color: var(--accent-secondary);
  box-shadow: 0 0 24px rgba(236, 72, 153, 0.2);
}
.weapon-name {
  font-family: var(--font-heading);
  color: var(--accent-secondary);
  font-weight: 700;
  margin-bottom: var(--space-xs);
}
.weapon-stats {
  color: var(--text-muted);
  font-size: 0.75rem;
  font-family: var(--font-body);
}

.enemy-card {
  background: var(--bg-elevated);
  border: 1px solid var(--border-subtle);
  border-radius: var(--radius-lg);
  padding: var(--space-md);
  text-align: center;
  transition: all var(--transition-base);
}
.enemy-card:hover {
  border-color: var(--danger);
  box-shadow: 0 0 24px rgba(239, 68, 68, 0.2);
}
.enemy-name {
  font-family: var(--font-heading);
  color: var(--danger);
  font-weight: 700;
  margin-bottom: var(--space-xs);
}

/* Controls Table */
.controls-table {
  width: 100%;
  border-collapse: collapse;
  margin: var(--space-lg) 0;
  font-family: var(--font-body);
  font-size: 0.875rem;
}
.controls-table th,
.controls-table td {
  padding: var(--space-sm) var(--space-md);
  text-align: left;
  border-bottom: 1px solid var(--border-subtle);
}
.controls-table th {
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.05em;
  font-size: 0.7rem;
  font-family: var(--font-ui);
  font-weight: 600;
}
.controls-table td {
  color: var(--text-secondary);
}
.controls-table td:first-child {
  color: var(--accent-primary);
  font-weight: 600;
}
.controls-table tr:hover td {
  background: var(--bg-surface);
}