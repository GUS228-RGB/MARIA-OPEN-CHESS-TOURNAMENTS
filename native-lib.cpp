// Ponte JNI: inicia o Node.js (nodejs-mobile) dentro do processo do aplicativo.
// Só uma instância do Node pode ser iniciada por processo; encerrar o app = encerrar o processo.
#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "node.h"

#define LOG_TAG "MariaOpenChess/node"

static int g_pipe[2] = {-1, -1};

// Lê o que o Node escreve em stdout/stderr e envia, linha a linha, para o logcat.
static void *logThread(void *) {
    char buf[1024];
    std::string line;
    ssize_t n;
    while ((n = read(g_pipe[0], buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                __android_log_write(ANDROID_LOG_INFO, LOG_TAG, line.c_str());
                line.clear();
            } else {
                line.push_back(buf[i]);
            }
        }
    }
    return nullptr;
}

static bool redirectOutputToLogcat() {
    static bool done = false;
    if (done) return true;
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    if (pipe(g_pipe) != 0) return false;
    dup2(g_pipe[1], STDOUT_FILENO);
    dup2(g_pipe[1], STDERR_FILENO);
    pthread_t t;
    if (pthread_create(&t, nullptr, logThread, nullptr) != 0) return false;
    pthread_detach(t);
    done = true;
    return true;
}

extern "C" JNIEXPORT jint JNICALL
Java_br_com_mariaopenchess_NodeBridge_startNodeWithArguments(JNIEnv *env, jobject /* this */,
                                                             jobjectArray arguments) {
    const jsize argc = env->GetArrayLength(arguments);

    // O libuv exige que todos os argumentos fiquem em um único bloco contínuo de memória.
    size_t total = 0;
    std::vector<std::string> args;
    args.reserve(argc);
    for (jsize i = 0; i < argc; i++) {
        auto js = reinterpret_cast<jstring>(env->GetObjectArrayElement(arguments, i));
        const char *c = env->GetStringUTFChars(js, nullptr);
        args.emplace_back(c);
        env->ReleaseStringUTFChars(js, c);
        env->DeleteLocalRef(js);
        total += args.back().size() + 1;
    }

    char *block = static_cast<char *>(calloc(total + 1, sizeof(char)));
    if (block == nullptr) return -1;
    std::vector<char *> argv(static_cast<size_t>(argc) + 1, nullptr);
    char *cursor = block;
    for (jsize i = 0; i < argc; i++) {
        memcpy(cursor, args[i].c_str(), args[i].size() + 1);
        argv[i] = cursor;
        cursor += args[i].size() + 1;
    }

    redirectOutputToLogcat();
    const int rc = node::Start(argc, argv.data());
    free(block);
    return rc;
}
