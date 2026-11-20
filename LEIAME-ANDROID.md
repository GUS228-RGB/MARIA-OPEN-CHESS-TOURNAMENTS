# maria open CHESS para Android (APK com Node.js embutido)

Aplicativo Android que traz o servidor dentro do próprio APK. O usuário instala, abre, toca em **CRIAR TORNEIO** e
**INICIAR HOSPEDAGEM**. Não precisa de Termux nem de Node instalado no celular.

> **Estado honesto:** este projeto foi escrito sem acesso ao Android SDK nem à internet, então **nunca foi compilado nem
> executado em um Android**. O que foi verificado: o código JavaScript (110 testes, inclusive com o motor SQLite alternativo),
> a sintaxe do C++ e dos XMLs, e a tela do app em viewport de celular. O que **não** foi verificado: a compilação do Kotlin
> e do Gradle, a ponte JNI com o nodejs-mobile, o serviço em primeiro plano e o comportamento no aparelho.
> Espere ter de corrigir pequenos erros no primeiro build. Se aparecerem, me envie a mensagem de erro.

## Como funciona
- `libnode.so` (nodejs-mobile, Node 18) é iniciado por JNI dentro do processo do app (`NodeBridge`, `native-lib.cpp`).
- O Node roda `src/mobile/main.js`, que sobe a tela CRIAR TORNEIO em `http://localhost:PORTA/`; um `WebView` mostra essa tela.
- Os participantes entram pelo navegador deles, pelo QR Code ou pelo endereço do Wi-Fi/hotspot, como nas outras versões.
- Um serviço em primeiro plano (`NodeService`) mantém o app vivo, mostra o estado na notificação e segura WakeLock/WifiLock
  enquanto há torneio hospedado.
- Como o Node 18 não tem `node:sqlite`, o banco usa **sql.js** (SQLite em WebAssembly) gravando o arquivo de forma atômica.
- O Node não lista as interfaces de rede no Android 11+; o app grava os IPs em `lan.json` e o servidor os lê.

## Requisitos para compilar
- Android Studio recente (Ladybug 2024.2 ou mais novo), com Android SDK 35, NDK e CMake 3.22.1 (o Studio oferece instalar).
- Node.js e npm no computador, com internet, só para o passo 1.

## Passo a passo
1. **Baixar o runtime** (libnode + sql.js), na raiz do projeto:
   ```
   node android/scripts/prepare.js
   ```
   Se o download do nodejs-mobile falhar, baixe `nodejs-mobile-v18.20.4-android.zip` em
   https://github.com/nodejs-mobile/nodejs-mobile/releases e rode com `--libnode-zip CAMINHO/arquivo.zip`.
2. Abra a pasta **`android/`** no Android Studio e aguarde a sincronização do Gradle.
3. Conecte o celular (depuração USB) e clique em **Run**, ou gere o APK:
   ```
   cd android && ./gradlew assembleDebug
   ```
   O APK fica em `android/app/build/outputs/apk/debug/app-debug.apk`. Copie para o celular e instale (permita "fontes desconhecidas").
4. Para distribuir, crie `android/keystore.properties` (`storeFile`, `storePassword`, `keyAlias`, `keyPassword`) e rode `./gradlew assembleRelease`.

## Uso no celular
- Na primeira abertura o app descompacta o servidor (alguns segundos) e pede permissão de notificação e de bateria.
- Mantenha a notificação "maria open CHESS" ativa durante o torneio e desative a economia de bateria para o app.
- **Outras redes:** nesta versão os participantes precisam estar no mesmo Wi-Fi ou conectados ao **ponto de acesso (hotspot)
  do celular do organizador**. O túnel pela internet (cloudflared/ssh) não existe dentro do APK.
- Dados em `filesDir/open-chess/torneios`. Use "Salvar cópia dos torneios em Downloads" na tela inicial para guardar um zip.
- Encerrar: botão "Encerrar aplicativo" ou ação "Encerrar" da notificação (faz backup e finaliza o processo).

## Limites conhecidos
- O nodejs-mobile permite uma só execução do Node por processo: encerrar o app finaliza o processo.
- Se o Android matar o processo, o torneio para; ele fica salvo e pode ser retomado em "Torneios salvos".
- O libnode 18 é compilado sem ICU (sem `Intl` no servidor). O código já evita isso (ex.: remoção de acentos manual).
- A Play Store exige, para apps novos, suporte a páginas de memória de 16 KB; o libnode pré-compilado pode não atender.
  Para instalar por APK direto isso não importa.
- Logs: `adb logcat -s MariaOpenChess MariaOpenChess/node`.
