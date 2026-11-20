# maria open CHESS

Plataforma para criar, hospedar e jogar torneios de xadrez. O organizador hospeda pelo computador
(`open-chess host`); os participantes entram pelo navegador, por URL ou QR Code, **sem instalar nada,
sem e-mail e sem senha**. Tudo é salvo em SQLite no dispositivo do organizador.

> **Estado atual (honesto):** o servidor, o CLI, a interface web (participantes e organizador), os
> sistemas de disputa, o motor de xadrez, as exportações e o banco estão prontos e testados no Linux.
> O nome do produto agora é **maria open CHESS** (o comando continua `open-chess`).
> **Ainda não existem** o aplicativo Android/iOS, os executáveis `.exe`/Linux autônomos e o APK.
> Veja [Status por plataforma](#status-por-plataforma).

## Requisitos
- Node.js **22.5 ou superior** (usa o SQLite nativo do Node; **nenhuma dependência npm**).

## Instalação e uso
```bash
git clone <repositório> && cd open-chess
npm link            # cria o comando "open-chess"  (ou use: node bin/open-chess.js ...)
open-chess host
```
No Windows use `bin\open-chess.cmd host` (ou `npm link`, que cria `open-chess`).

## Pacotes (Linux, Windows, Android)
`node scripts/package.js` gera em `dist/` (com `SHA256SUMS.txt`):
- `…-linux.tar.gz`: `open-chess`, `abrir-maria-open-chess.sh`, `instalar.sh` (atalho no menu), LEIAME.
- `…-windows.zip`: `Abrir maria open CHESS.cmd` (dois cliques), `open-chess.cmd`, `bootstrap-node.ps1`, LEIAME.
- `…-android-termux.zip`: `instalar-termux.sh` (instala no Termux), LEIAME.

Os lançadores usam o Node ≥ 22.5 do sistema ou, com a sua confirmação, baixam o Node oficial e **verificam o SHA-256**.
Não são binários únicos (.exe/.apk): ver tabela de status abaixo.

### Android: APK com Node embutido (projeto para o Android Studio)
`dist/…-android-studio-projeto.zip` traz o projeto Kotlin que embute o Node.js (nodejs-mobile) e mostra a tela CRIAR TORNEIO
num WebView, com serviço em primeiro plano. Passo a passo em `android/LEIAME-ANDROID.md`.
**Nunca foi compilado nem executado em Android** (o ambiente de desenvolvimento não tem o Android SDK).
No Node 18 do nodejs-mobile o banco usa sql.js (`src/db/sqljs-driver.js`); `npm run test:sqljs` roda toda a suíte com ele
(usando um dublê do sql.js, porque o pacote real vem do npm).

## Comandos
| Comando | O que faz |
|---|---|
| `open-chess host` | Perguntas interativas (nome, descrição, vagas, modalidade, sistema, rodadas, tempo, pontuação, mesas, desempate, porta) e inicia o servidor. Mostra IP, porta, URL, QR Code, participantes, vagas, rodada, partidas e estado, ao vivo. `q` ou Ctrl+C encerra com segurança. |
| `open-chess app` | Abre no navegador do próprio aparelho a tela **CRIAR TORNEIO** (limite de participantes, modalidade, sistema…) com o botão **INICIAR HOSPEDAGEM**, QR Code e contador ao vivo. Escuta só em `127.0.0.1`. É a tela usada no Android (Termux) e nos atalhos de dois cliques. |
| `open-chess status` | Mostra o estado do torneio hospedado neste computador (outro terminal). |
| `open-chess stop` | Encerra a hospedagem salvando tudo (backup final; TXT se o torneio terminou). |
| `open-chess restore <backup.db>` | Restaura um backup (com o servidor parado). Guarda cópia do estado atual antes. |
| `open-chess help` | Ajuda. |

Opções do `host`: `--port N`, `--local`, `--tunnel [auto|cloudflared|ssh]`, `--remote-admin`, `--resume [pasta]`, `--config arquivo.json`, `--yes` e
`--name --description --max --modality --system --rounds --time --inc --win --draw --loss --tables --tiebreaks`
(respondem às perguntas, para uso em scripts).
Ao reabrir `host`, torneios salvos não finalizados podem ser **retomados** (recuperação após reinício).

Dados em `~/open-chess/torneios/<torneio>/` (`OPEN_CHESS_HOME` muda o local): `torneio.db`,
`backups/`, `logs/servidor.log` e `<Nome>_resultados.txt`.

## Como os participantes entram
1. O organizador compartilha a URL (ex.: `http://192.168.0.10:8787/`) ou o QR Code.
2. O participante toca em **PARTICIPAR** e informa só o nome. Nomes repetidos são permitidos; cada um recebe ID próprio (#001, #002…).
3. Quando as vagas acabam, a página mostra "torneio lotado". Tudo atualiza em tempo real (WebSocket).

## Acesso de outras redes (internet)
O IP local (`http://192.168.x.x:8787/`) só funciona para quem está na **mesma** rede Wi-Fi/Ethernet.
Para jogadores em **outras redes**, abra um túnel seguro (HTTPS/WSS), gratuito e sem conta:

```bash
open-chess host --tunnel            # ou responda "outras redes" na pergunta do host
```
- O túnel usa o que existir no computador: **cloudflared** (Cloudflare Quick Tunnel) ou o **ssh** (localhost.run; o OpenSSH já vem no Windows 10/11 e no Linux). Se nenhum existir, o `host` mostra como instalar e segue só na rede local.
- Surge uma **URL pública** `https://....` que vale de qualquer rede. Ela aparece no terminal, no `status`, no painel e no **QR Code**.
- O servidor e os dados continuam **no computador do organizador**, que precisa ficar ligado, com o programa aberto e com internet. Se o túnel cair, ele tenta reabrir; nesse caso a URL **muda** e é preciso compartilhar a nova.
- **Segurança:** pela internet só a área dos participantes fica aberta. O painel do organizador e as rotas de controle ficam **bloqueados** no acesso pelo túnel; use o endereço local para administrar (ou `--remote-admin`, por sua conta e risco). Inscrições têm limite por IP real do cliente.
- Alternativas sem túnel embutido: Tailscale/ZeroTier (rede privada entre os aparelhos) ou redirecionar a porta no roteador (exige IP público; não recomendado).
- Se um dispositivo da mesma rede não abrir o endereço local, libere a porta no firewall (`sudo ufw allow 8787/tcp` no Linux; o `host` mostra o comando do Windows).

## Painel do organizador
Abra o endereço `Painel do organizador` mostrado no terminal (`/admin#k=<chave>`). A chave é gerada a cada
inicialização, guardada só como hash, e vira um cookie de sessão (HttpOnly, SameSite=Strict). Há limite de
tentativas. Participantes comuns nunca têm acesso às rotas `/api/admin/*`.
Recursos: abrir/pausar/encerrar inscrições, adicionar/remover participantes, iniciar torneio, iniciar/finalizar rodadas,
gerar próxima rodada, lançar resultados (presencial), **corrigir com justificativa** (fica no histórico),
adiar/cancelar/reativar partidas, editar e trocar confrontos antes da rodada, anexar lances SAN a partidas presenciais,
configurações, TXT/PGN, importação de PGN, backup, verificação de integridade e registro de auditoria.

## Regras e modalidades
- **Motor de xadrez próprio e validado no servidor:** lances legais, xeque, mate, roque, en passant, promoção,
  afogamento, repetição, regra dos 50 lances, material insuficiente, abandono, empate por acordo e relógio.
  Só os dois jogadores da partida podem mover; o navegador nunca é fonte de verdade.
- **Presencial:** mesas, cores, resultado manual, correção autorizada, histórico. Lances só existem se informados (nada é inventado).
- **Online:** jogo no navegador com relógio, reconexão automática (inclusive após reiniciar o servidor) e salvamento a cada lance.
- **Híbrido:** partidas online e presenciais no mesmo torneio (marque o tipo em "Editar confronto" antes da rodada).
- **Sistemas:** Suíço, Round Robin (turno único/duplo), Eliminatória simples e dupla.
- **Classificação:** pontos configuráveis, Buchholz, Buchholz Cut-1, Sonneborn-Berger, vitórias, progressivo e rating, em ordem configurável.

## Exportações
TXT de resultados (gerado automaticamente ao finalizar, na pasta do torneio, e baixável pelo navegador),
PGN por partida e do torneio, importação e reprodução de PGN.

## Arquitetura
Escolhi **Node.js (JavaScript) em vez de Flutter/Dart**: o ambiente de desenvolvimento não tinha Flutter nem Android SDK,
e um núcleo único em JS roda no servidor, no CLI e (via navegador) em todas as telas, sem motores diferentes por plataforma.
```
src/chess      motor de regras, relógio, PGN         src/core    torneio, emparelhamento, classificação, QR, partidas online
src/db         SQLite (transacional, backup)         src/server  HTTP + WebSocket (RFC 6455 próprio)
src/export     TXT e PGN                              src/cli     host / status / stop / restore / help
web/           interface (JS puro, sem build)         test/       testes automatizados
```

## Testes
```bash
npm test          # 110 testes: regras, sistemas de disputa, desempates, SQLite, servidor HTTP/WS, reconexão, TXT/PGN
npm run test:e2e  # fluxos reais em navegadores (requer Playwright)
```

## Status por plataforma
| Item | Situação |
|---|---|
| Servidor + CLI no **Linux** | Testado (Node 22, Linux). |
| Interface web em Chromium (desktop e viewport de celular) | Testada. Firefox/Safari **não** testados. |
| Pacote **Linux** (`.tar.gz`) | Testado aqui: lançador, `host/status/stop`, `instalar.sh`, download do Node com verificação de hash (usando espelho local simulado). Não testado em Mint/Ubuntu reais. |
| Pacote **Windows** (`.zip`, `.cmd` + PowerShell) | **Não testado no Windows.** Escrito sem acesso a uma máquina Windows. |
| **Android** via Termux (`instalar-termux.sh`) | Servidor, tela CRIAR TORNEIO/INICIAR HOSPEDAGEM e instalador testados em Linux (viewport de celular, Termux simulado). **Não testado em aparelho Android.** O servidor só roda com o Termux aberto. **Não é um APK.** |
| **Android APK** (projeto do Android Studio) | Código JS testado (inclusive com o motor sql.js simulado); C++ e XMLs validados; **Kotlin/Gradle/JNI nunca compilados**, nada testado em aparelho. Sem túnel para outras redes (só Wi-Fi/hotspot). |
| **iOS** | **Não implementado.** iOS não permite hospedar servidor em segundo plano; só participar pelo navegador. |
| Executáveis `.exe`/ELF, APK, projeto iOS | **Não gerados** (sem SDKs/rede no ambiente de desenvolvimento). |
| Acesso por outras redes (`--tunnel`) | Lógica testada com programa de túnel **simulado** (95 testes). **Não testado com túnel real**: o ambiente de desenvolvimento não tinha internet livre. |
| Arrastar peças com o dedo/mouse | Só clique/toque e teclado por enquanto. |
