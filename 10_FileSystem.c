
#include <stdio.h>
#include <ctype.h>  // isspace()

int main() {
    FILE *fp = fopen("input.txt", "r");
    if (!fp) {
        perror("File open failed");
        return 1;
    }

    char ch;
    int char_count = 0;  // 공백 제외 문자 수
    int word_count = 0;
    int in_word = 0;     // 단어 내부에 있는지 상태 추적

    while ((ch = fgetc(fp)) != EOF) {
        // 문자 수 증가 (공백 제외)
        if (!isspace((unsigned char)ch))
            char_count++;

        // 단어 경계 판단
        if (isspace((unsigned char)ch)) {
            in_word = 0;
        } else if (in_word == 0) {
            word_count++;
            in_word = 1;
        }
    }

    fclose(fp);

    printf("Characters (excluding whitespace): %d\n", char_count);
    printf("Words: %d\n", word_count);

    return 0;
}

