#include <stdio.h>
#include <stdlib.h>

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8
#define MAX_QUEUE_SIZE 1000

typedef struct Square {
    int xCoord;
    int yCoord;
} Square;

typedef struct PathNode {
    Square* child;
    struct PathNode* parent;
} PathNode;

typedef struct Knight {
    Square* position;
} Knight;

typedef struct KnightPathFinderBFS {
    PathNode** queue;
    int queueSize;
    Square** visited;
    int visitedSize;
    Square* startSquare;
    Square* endSquare;
    Knight* piece;
} KnightPathFinderBFS;

Square* createSquare(int x, int y) {
    Square* square = (Square*)malloc(sizeof(Square));
    square->xCoord = x;
    square->yCoord = y;
    return square;
}

Knight* createKnight(Square* position) {
    Knight* knight = (Knight*)malloc(sizeof(Knight));
    knight->position = position;
    return knight;
}

void addQueue(KnightPathFinderBFS* search, PathNode* pathNode) {
    search->queue[search->queueSize++] = pathNode;
}

int isVisited(KnightPathFinderBFS* search, Square* square) {
    for (int i = 0; i < search->visitedSize; i++) {
        if (search->visited[i]->xCoord == square->xCoord && search->visited[i]->yCoord == square->yCoord) {
            return 1;
        }
    }
    return 0;
}

void markVisited(KnightPathFinderBFS* search, Square* square) {
    search->visited[search->visitedSize++] = square;
}

void validMove(Knight* knight, Square* currentSquare, Square** moves, int* moveCount) {
    int dx[] = {2, 2, -2, -2, 1, 1, -1, -1};
    int dy[] = {1, -1, 1, -1, 2, -2, 2, -2};

    *moveCount = 0;
    for (int i = 0; i < 8; i++) {
        int newX = currentSquare->xCoord + dx[i];
        int newY = currentSquare->yCoord + dy[i];
        if (newX >= 0 && newX < BOARD_WIDTH && newY >= 0 && newY < BOARD_HEIGHT) {
            moves[(*moveCount)++] = createSquare(newX, newY);
        }
    }
}

void printSquare(Square* position) {
    char file = 'A' + position->yCoord; // Column letter (A to H)
    int rank = 1 + position->xCoord;    // Row number (1 to 8)
    printf("%c%d", file, rank);
}

void printBoard(Square* position) {
    printf("\n   "); // Leading space for row labels
    for (char file = 'A'; file <= 'H'; file++) {
        printf(" %c ", file); // Print column labels A to H
    }
    printf("\n");

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        printf(" %d ", i + 1); // Print row labels 1 to 8

        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (position->xCoord == i && position->yCoord == j) {
                printf(" K "); // Knight's current position
            } else {
                printf(" . ");
            }
        }
        printf("\n");
    }
    printf("\n");
}


void generateAction(KnightPathFinderBFS* search) {
    PathNode* startPathNode = (PathNode*)malloc(sizeof(PathNode));
    startPathNode->child = search->startSquare;
    startPathNode->parent = NULL;
    addQueue(search, startPathNode);
    markVisited(search, search->startSquare);

    while (search->queueSize > 0) {
        PathNode* current = search->queue[0];
        for (int i = 0; i < search->queueSize - 1; i++) {
            search->queue[i] = search->queue[i + 1];
        }
        search->queueSize--;

        Square* buffer = current->child;
        if (buffer->xCoord == search->endSquare->xCoord && buffer->yCoord == search->endSquare->yCoord) {
            printf("The shortest path from ");
            printSquare(search->startSquare);
            printf(" to ");
            printSquare(search->endSquare);
            printf(" is: ");

            PathNode* pathNode = current;
            int pathLength = 0;
            while (pathNode != NULL) {
                pathLength++;
                pathNode = pathNode->parent;
            }

            Square* path[pathLength];
            pathNode = current;
            for (int i = pathLength - 1; i >= 0; i--) {
                path[i] = pathNode->child;
                pathNode = pathNode->parent;
            }

            for (int i = 0; i < pathLength; i++) {
                printSquare(path[i]);
                printf(" ");
            }
            printf("\n");

            for (int i = 0; i < pathLength; i++) {
                printf("\nThe Knight moves to ");
                printSquare(path[i]);
                printf(":\n");
                printBoard(path[i]);
            }
            return;
        }

        Square* moves[8];
        int moveCount = 0;
        validMove(search->piece, buffer, moves, &moveCount);

        for (int i = 0; i < moveCount; i++) {
            if (!isVisited(search, moves[i])) {
                PathNode* newPathNode = (PathNode*)malloc(sizeof(PathNode));
                newPathNode->child = moves[i];
                newPathNode->parent = current;
                addQueue(search, newPathNode);
                markVisited(search, moves[i]);
            } else {
                free(moves[i]);
            }
        }
    }
}

Square* convertChessNotation(char file, int rank) {
    int x = rank - 1;          // Convert rank to x-coordinate
    int y = file - 'A';        // Convert file to y-coordinate
    if (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
        return createSquare(x, y);
    } else {
        printf("Invalid chess notation.\n");
        exit(1);
    }
}

int main() {
    char startFile, endFile;
    int startRank, endRank;

    printf("Enter starting position: ");
    scanf(" %c%d", &startFile, &startRank);

    printf("Enter ending position: ");
    scanf(" %c%d", &endFile, &endRank);

    Square* startSquare = convertChessNotation(startFile, startRank);
    Square* endSquare = convertChessNotation(endFile, endRank);

    Knight* knight = createKnight(startSquare);
    KnightPathFinderBFS* search = (KnightPathFinderBFS*)malloc(sizeof(KnightPathFinderBFS));
    search->queue = (PathNode**)malloc(MAX_QUEUE_SIZE * sizeof(PathNode*));
    search->visited = (Square**)malloc(MAX_QUEUE_SIZE * sizeof(Square*));
    search->queueSize = 0;
    search->visitedSize = 0;
    search->startSquare = startSquare;
    search->endSquare = endSquare;
    search->piece = knight;

    generateAction(search);

    free(startSquare);
    free(endSquare);
    free(knight);
    free(search->queue);
    free(search->visited);
    free(search);

    return 0;
}
