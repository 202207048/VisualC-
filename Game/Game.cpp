// Game.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Game.h"
#include "time.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib");
#pragma comment(lib, "Msimg32.lib"); //TransparentBlt(아이콘 배경 투명하게 처리)
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAME));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = NULL;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAME);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle,
       WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,  // 최대화 버튼과 크기 조절 제거
       CW_USEDEFAULT, 0, 800, 600,  // 원하는 크기로 설정 (너비 800, 높이 600)
       nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

RECT player, food1, food2, score, score2, enemy, enemy2;
int player_size = 100;  /// 플레이어 크기
int player_life = 3;    ///플레이어 생명력
int player_score = 0;
int food_size = 50;
int enemy_size = 100;
int score_size = 50;
int fall_speed = 5;
//눈동자 움직임
int lookdir = 0; //0 = 정면 7 = 오른쪽 -7 = 왼쪽
BOOL g_gameover;        ///게임 오버
BOOL g_rare = FALSE;            ///안씀
int player_speed = 30;  ///플레이어 이동 속도
//키보드 눌림 상태 저장
BOOL key_left = FALSE;
BOOL key_right = FALSE;
HBITMAP hBgImages[4]; ///배경
HBITMAP hCoinBmp; ///점수 코인 사진
HBITMAP hEnemyBmp;  ///적 사진
HBITMAP hPlayerBmp; ///플레이어 사진
HBITMAP hFoodBmp;   ///음식 사진
HBITMAP hFood2Bmp;  ///음식 사진 2



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        player.left = 350; ///창 사이즈 800 x 600 고정 후 (800 - player_size) / 2
        player.top = 440;   ///(600 - player_size) / 2 지만 바닥에 딱 붙는게 싫어서 값을 줄여놓음
        player.right = player.left + player_size;
        player.bottom = player.top + player_size;
        srand(time(NULL));  ///랜덤 시드값 변경

        food1.left = rand() % (800 - food_size);
        food1.top = 0;
        food1.right = food1.left + food_size;
        food1.bottom = food1.top + food_size;

        food2.left = rand() % (800 - food_size);
        food2.top = 0;
        food2.right = food2.left + food_size;
        food2.bottom = food2.top + food_size;

        enemy.left = rand() % (800 - enemy_size);
        enemy.top = 0;
        enemy.right = enemy.left + enemy_size;
        enemy.bottom = enemy.top + enemy_size;

        enemy2.left = rand() % (800 - enemy_size);
        enemy2.top = 0;
        enemy2.right = enemy2.left + enemy_size;
        enemy2.bottom = enemy2.top + enemy_size;

        score.left = rand() % (800 - score_size);
        score.top = 0;
        score.right = score.left + score_size;
        score.bottom = score.top + score_size;

        score2.left = rand() % (800 - score_size);
        score2.top = 0;
        score2.right = score2.left + score_size;
        score2.bottom = score2.top + score_size;


        SetTimer(hWnd, 1, 30, NULL);
        //bgm, 타격음
        mciSendString(L"open \"background.mp3\" type mpegvideo alias my_bgm", NULL, 0, NULL);
        mciSendString(L"open \"hit.mp3\" type mpegvideo alias my_hit", NULL, 0, NULL);
        mciSendString(L"open \"coin.mp3\" type mpegvideo alias my_coin", NULL, 0, NULL);

        mciSendString(L"play my_bgm repeat", NULL, 0, NULL);

        //비트맵
        hBgImages[0] = (HBITMAP)LoadImage(NULL, L"white.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
        hBgImages[1] = (HBITMAP)LoadImage(NULL, L"progress2.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
        hBgImages[2] = (HBITMAP)LoadImage(NULL, L"progress4.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
        hBgImages[3] = (HBITMAP)LoadImage(NULL, L"progress3.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
        
        

        if (hBgImages[0] == NULL)
        {
            MessageBox(hWnd, L"이미지 로드 실패! white.bmp 파일확인!", L"에러", MB_OK);
        }

        hCoinBmp = (HBITMAP)LoadImage(NULL, L"coin.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        
        if (hCoinBmp == NULL)
        {
            MessageBox(hWnd, L"코인 이미지 로드 실패", L"에러", MB_OK);
        }

        hEnemyBmp = (HBITMAP)LoadImage(NULL, L"enemy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

        if (hEnemyBmp == NULL)
        {
            MessageBox(hWnd, L"적 이미지 로드 실패", L"에러", MB_OK);
        }

        hPlayerBmp = (HBITMAP)LoadImage(NULL, L"player.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

        if (hPlayerBmp == NULL)
        {
            MessageBox(hWnd, L"플레이어 이미지 로드 실패", L"에러", MB_OK);
        }
        
    }

    hFoodBmp = (HBITMAP)LoadImage(NULL, L"food1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (hFoodBmp == NULL)
    {
        MessageBox(hWnd, L"음식 이미지 로드 실패", L"에러", MB_OK);
    }

    hFood2Bmp = (HBITMAP)LoadImage(NULL, L"food2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (hFood2Bmp == NULL)
    {
        MessageBox(hWnd, L"음식 이미지 2 로드 실패", L"에러", MB_OK);
    }
        break;

    case WM_TIMER:
    {
        // 좌우 이동
        if (key_left == TRUE)
        {
            player.left -= player_speed;
            player.right -= player_speed;
        }
        if (key_right == TRUE)
        {
            player.left += player_speed;
            player.right += player_speed;
        }

        // 점수에 따른 떨구기 속도 조절
        if (player_score < 200)
        {
            fall_speed = 5;
        }
        else if (player_score < 500)
        {
            fall_speed = 10;
        }
        else if (player_score < 1000)
        {
            fall_speed = 15;
        }
        else
        {
            fall_speed = 18;
        }

        ///음식 1 떨구기
        food1.top += fall_speed;
        food1.bottom += fall_speed;

        if (food1.top > 600)
        {
            food1.left = rand() % (800 - food_size);
            food1.top = -200;
            food1.right = food1.left + food_size;
            food1.bottom = food1.top + food_size;
        }

        //음식 2 떨구기
        food2.top += fall_speed;
        food2.bottom += fall_speed;

        if (food2.top > 600)
        {
            food2.left = rand() % (800 - food_size);
            food2.top = -200;
            food2.right = food2.left + food_size;
            food2.bottom = food2.top + food_size;
        }
        //적 떨구기
        enemy.top += fall_speed;
        enemy.bottom += fall_speed;

        if (enemy.top > 600)
        {
            enemy.left = rand() % (800 - enemy_size);
            enemy.top = -200;
            enemy.right = enemy.left + enemy_size;
            enemy.bottom = enemy.top + enemy_size;
        }
        enemy2.top += fall_speed;
        enemy2.bottom += fall_speed;

        if (enemy2.top > 600)
        {
            enemy2.left = rand() % (800 - enemy_size);
            enemy2.top = -200;
            enemy2.right = enemy2.left + enemy_size;
            enemy2.bottom = enemy2.top + enemy_size;
        }

        score.top += fall_speed;
        score.bottom += fall_speed;

        if (score.top > 600)
        {
            score.left = rand() % (800 - score_size);
            score.top = -200;
            score.right = score.left + score_size;
            score.bottom = score.top + score_size;
        }

        score2.top += fall_speed;
        score2.bottom += fall_speed;

        if (score2.top > 600)
        {
            score2.left = rand() % (800 - score_size);
            score2.top = -200;
            score2.right = score2.left + score_size;
            score2.bottom = score2.top + score_size;
        }

        RECT ret;
        if (IntersectRect(&ret, &player, &enemy)) //적과 부딛히면 생명력 -
        {
            enemy.left = rand() % (800 - enemy_size);
            enemy.top = -200;
            enemy.right = enemy.left + enemy_size;
            enemy.bottom = enemy.top + enemy_size;
            player_life--;
            //피격음
            mciSendString(L"play my_hit from 0", NULL, 0, NULL);

            if (0 > player_life)
            {
                player_life = 0;
                KillTimer(hWnd, 1);
                g_gameover = TRUE;
                mciSendString(L"stop my_bgm", NULL, 0, NULL); //죽었을 시 bgm 멈추기
                MessageBox(hWnd, L"생명 소진", L"게임 오버", MB_OK);
            }
            else
            {
                mciSendString(L"play my_hit from 0", NULL, 0, NULL); //아니면 다시 재생
            }
        }
        if (IntersectRect(&ret, &player, &enemy2)) //적과 부딛히면 생명력 -
        {
            enemy2.left = rand() % (800 - enemy_size);
            enemy2.top = -200;
            enemy2.right = enemy2.left + enemy_size;
            enemy2.bottom = enemy2.top + enemy_size;
            player_life--;
            //피격음
            mciSendString(L"play my_hit from 0", NULL, 0, NULL);
        }
        if (IntersectRect(&ret, &player, &food1)) //음식을 먹으면 생명력 +1
        {
            food1.left = rand() % (800 - food_size);
            food1.top = -200;
            food1.right = food1.left + food_size;
            food1.bottom = food1.top + food_size;
            player_life++;
            if (3 < player_life)
            {
                player_life = 3; //최대 생명력 3으로 고정
            }
        }
        if (IntersectRect(&ret, &player, &food2)) //음식 2 먹으면 생명력 +1
        {
            food2.left = rand() % (800 - food_size);
            food2.top = -200;
            food2.right = food2.left + food_size;
            food2.bottom = food2.top + food_size;
            player_life++;
            if (3 < player_life)
            {
                player_life = 3; //최대 생명력 3으로 고정
            }
        }
        if (IntersectRect(&ret, &player, &score)) //점수 + 20
        {

            score.left = rand() % (800 - score_size);
            score.top = -200;
            score.right = score.left + score_size;
            score.bottom = score.top + score_size;
            player_score += 20;

            //코인 먹는 소리
            mciSendString(L"play my_coin from 0", NULL, 0, NULL);

            
        }

        if (IntersectRect(&ret, &player, &score2))
        {
            score2.left = rand() % (800 - score_size);
            score2.top = -200;
            score2.right = score2.left + score_size;
            score2.bottom = score2.top + score_size;
            player_score += 20;

            mciSendString(L"play my_coin from 0", NULL, 0, NULL);
        }

        //상관 없는것들끼리 부딪히면 위치 초기화 (겹치지 않게)

        if (IntersectRect(&ret, &food1, &food2))
        {
            food1.left = rand() % (800 - food_size);
            food1.top = -200;
            food1.right = food1.left + food_size;
            food1.bottom = food1.top + food_size;
        }
        if (IntersectRect(&ret, &food1, &score))
        {
            food1.left = rand() % (800 - food_size);
            food1.top = -200;
            food1.right = food1.left + food_size;
            food1.bottom = food1.top + food_size;
        }
        if (IntersectRect(&ret, &food1, &enemy))
        {
            food1.left = rand() % (800 - food_size);
            food1.top = -200;
            food1.right = food1.left + food_size;
            food1.bottom = food1.top + food_size;
        }
        if (IntersectRect(&ret, &food2, &enemy))
        {
            food2.left = rand() % (800 - food_size);
            food2.top = -200;
            food2.right = food2.left + food_size;
            food2.bottom = food2.top + food_size;
        }
        if (IntersectRect(&ret, &food2, &score))
        {
            food2.left = rand() % (800 - food_size);
            food2.top = -200;
            food2.right = food2.left + food_size;
            food2.bottom = food2.top + food_size;
        }
        if (IntersectRect(&ret, &enemy, &score))
        {
            score.left = rand() % (800 - score_size);
            score.top = -200;
            score.right = score.left + score_size;
            score.bottom = score.top + score_size;  
        }
        if (IntersectRect(&ret, &enemy, &enemy2))
        {
            enemy2.left = rand() % (800 - enemy_size);
            enemy2.top = -200;
            enemy2.right = enemy2.left + enemy_size;
            enemy2.bottom = enemy2.top + enemy_size;
        }

        if (IntersectRect(&ret, &food1, &enemy2))
        {
            enemy2.left = rand() % (800 - enemy_size);
            enemy2.top = -200;
            enemy2.right = enemy2.left + enemy_size;
            enemy2.bottom = enemy2.top + enemy_size;
        }

        if (IntersectRect(&ret, &food2, &enemy2))
        {
            enemy2.left = rand() % (800 - enemy_size);
            enemy2.top = -200;
            enemy2.right = enemy2.left + enemy_size;
            enemy2.bottom = enemy2.top + enemy_size;
        }
        if (IntersectRect(&ret, &score, &enemy2))
        {
            enemy2.left = rand() % (800 - enemy_size);
            enemy2.top = -200;
            enemy2.right = enemy2.left + enemy_size;
            enemy2.bottom = enemy2.top + enemy_size;
        }

        if (IntersectRect(&ret, &score, &score2))
        {
            score2.left = rand() % (800 - score_size);
            score2.top = -200;
            score2.right = score2.left + score_size;
            score2.bottom = score2.top + score_size;
        }

        if (IntersectRect(&ret, &food1, &score2))
        {
            score2.left = rand() % (800 - score_size);
            score2.top = -200;
            score2.right = score2.left + score_size;
            score2.bottom = score2.top + score_size;
        }

        if (IntersectRect(&ret, &food2, &score2))
        {
            score2.left = rand() % (800 - score_size);
            score2.top = -200;
            score2.right = score2.left + score_size;
            score2.bottom = score2.top + score_size;
        }

        if (IntersectRect(&ret, &enemy, &score2))
        {
            score2.left = rand() % (800 - score_size);
            score2.top = -200;
            score2.right = score2.left + score_size;
            score2.bottom = score2.top + score_size;
        }
        if (IntersectRect(&ret, &enemy2, &score2))
        {
            score2.left = rand() % (800 - score_size);
            score2.top = -200;
            score2.right = score2.left + score_size;
            score2.bottom = score2.top + score_size;
        }

        // 오른쪽 끝으로 나갈 시 왼쪽으로 텔레포트
        if (player.left > 800)
        {
            player.left = 0;
            player.right = 100;
        }

        if (player.right < 0)
        {
            player.right = 800;
            player.left = 700;
        }

        InvalidateRect(hWnd, NULL, FALSE);
    }
        break;
    case WM_KEYDOWN:
    {

        if (wParam == 'R' || wParam == 'r')
        {
            if (g_gameover == TRUE)
            {
                // 변수 초기화
                player_score = 0;
                player_life = 3;
                fall_speed = 5;
                g_gameover = FALSE; // 게임 재개

                // 위치 초기화
               // 적(Enemy) 초기화 (크기: enemy_size = 100)
                enemy.left = rand() % (800 - enemy_size);
                enemy.top = 0;
                enemy.right = enemy.left + enemy_size;   // 왼쪽 + 100
                enemy.bottom = enemy.top + enemy_size;   // 위 + 100 (정사각형 유지)

                // 음식1(Food1) 초기화 (크기: food_size = 50)
                food1.left = rand() % (800 - food_size);
                food1.top = 0; 
                food1.right = food1.left + food_size;
                food1.bottom = food1.top + food_size;

                // 음식2(Food2) 초기화
                food2.left = rand() % (800 - food_size);
                food2.top = 0;
                food2.right = food2.left + food_size;
                food2.bottom = food2.top + food_size;

                // 점수 아이템(Score) 초기화 (크기: score_size = 50)
                score.left = rand() % (800 - score_size);
                score.top = 0;
                score.right = score.left + score_size;
                score.bottom = score.top + score_size;

                score2.left = rand() % (800 - score_size);
                score2.top = 0;
                score2.right = score2.left + score_size;
                score2.bottom = score2.top + score_size;

                // 플레이어 위치 초기화
                player.left = 350;
                player.top = 440;
                player.right = 450; // player_size가 100이라면
                player.bottom = 540;

                //눈 위치 초기화
                lookdir = 0;

                // 키보드 상태 초기화 (중요)
                key_left = FALSE;
                key_right = FALSE;

                mciSendString(L"play my_bgm from 0 repeat", NULL, 0, NULL);

                // 타이머 재시작 (가장 중요)
                SetTimer(hWnd, 1, 30, NULL);

                InvalidateRect(hWnd, NULL, FALSE);
            }
        }

        if (g_gameover)
            break;
        
    

        switch (wParam)
        {
        case VK_LEFT:
        {
            key_left = TRUE;
            
            lookdir = -7;
        }
        break;
        case VK_RIGHT:
        {
            key_right = TRUE;
            
            lookdir = 7;
        }
        break;
        }

        InvalidateRect(hWnd, NULL, FALSE);
    }
    
        break;
    case WM_KEYUP: //키 미입력시 눈 제자리로
    {
        switch (wParam)
        {
        case VK_LEFT:
        {
            key_left = FALSE;
        }
            break;
        case VK_RIGHT:
        {
            key_right = FALSE;
        }
            break;
        }
        lookdir = 0;
    }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            HDC hMemDC = CreateCompatibleDC(hdc);
            int imgIndex = 0;

            int cycle_score = player_score % 400;

            if (cycle_score < 150)
            {
                imgIndex = 0;
            }
            else if (cycle_score < 200)
            {
                imgIndex = 1;
            }
            else if (cycle_score < 350)
            {
                imgIndex = 2;
            }
            else
            {
                imgIndex = 3;
            }

            HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBgImages[imgIndex]);

            BitBlt(hdc, 0, 0, 800, 600, hMemDC, 0, 0, SRCCOPY);

            SelectObject(hMemDC, hOldBitmap);
            DeleteDC(hMemDC);

            ///코인 부분
            HDC hCoinDC = CreateCompatibleDC(hdc);
            HBITMAP hOldCoinBmp = (HBITMAP)SelectObject(hCoinDC, hCoinBmp);

            BITMAP bmpInfo;
            GetObject(hCoinBmp, sizeof(BITMAP), &bmpInfo);
            //코인 그리기
            TransparentBlt(hdc, score.left, score.top, score.right - score.left, score.bottom - score.top, hCoinDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, RGB(255, 255, 255));
            TransparentBlt(hdc, score2.left, score2.top, score2.right - score2.left, score2.bottom - score2.top, hCoinDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, RGB(255, 255, 255));

            SelectObject(hCoinDC, hOldCoinBmp);
            DeleteObject(hCoinDC);
            //적 부분
            HDC hEnemyDC = CreateCompatibleDC(hdc);
            HBITMAP hOldEnemyBmp = (HBITMAP)SelectObject(hEnemyDC, hEnemyBmp);

            BITMAP enemyInfo;
            GetObject(hEnemyBmp, sizeof(BITMAP), &enemyInfo);

            //적 그리기
            StretchBlt(hdc, enemy.left, enemy.top, enemy.right - enemy.left, enemy.bottom - enemy.top, hEnemyDC, 0, 0, enemyInfo.bmWidth, enemyInfo.bmHeight, SRCCOPY);
            StretchBlt(hdc, enemy2.left, enemy2.top, enemy2.right - enemy2.left, enemy2.bottom - enemy2.top, hEnemyDC, 0, 0, enemyInfo.bmWidth, enemyInfo.bmHeight, SRCCOPY);

            SelectObject(hEnemyDC, hOldEnemyBmp);
            DeleteObject(hEnemyDC);

            //플레이어 그리기
            HDC hPlayerDC = CreateCompatibleDC(hdc);
            HBITMAP hOldPlayerBmp = (HBITMAP)SelectObject(hPlayerDC, hPlayerBmp);

            BITMAP playerInfo;
            GetObject(hPlayerBmp, sizeof(BITMAP), &playerInfo);

            TransparentBlt(hdc, player.left, player.top, player.right - player.left, player.bottom - player.top, hPlayerDC, 0, 0, playerInfo.bmWidth, playerInfo.bmHeight, RGB(255, 255, 255));
            SelectObject(hPlayerDC, hOldPlayerBmp);
            DeleteObject(hPlayerDC);

            //음식 1
            HDC hFoodDC = CreateCompatibleDC(hdc);
            HBITMAP hOldFoodBmp = (HBITMAP)SelectObject(hFoodDC, hFoodBmp);

            BITMAP foodInfo;
            GetObject(hFoodBmp, sizeof(BITMAP), &foodInfo);

            TransparentBlt(hdc, food1.left, food1.top, food1.right - food1.left, food1.bottom - food1.top, hFoodDC, 0, 0, foodInfo.bmWidth, foodInfo.bmHeight, RGB(255, 255, 255));
            SelectObject(hFoodDC, hOldFoodBmp);
            DeleteObject(hFoodDC);

            HDC hFood2DC = CreateCompatibleDC(hdc);
            HBITMAP hOldFood2Bmp = (HBITMAP)SelectObject(hFood2DC, hFood2Bmp);

            BITMAP food2Info;
            GetObject(hFood2Bmp, sizeof(BITMAP), &food2Info);

            TransparentBlt(hdc, food2.left, food2.top, food2.right - food2.left, food2.bottom - food2.top, hFood2DC, 0, 0, food2Info.bmWidth, food2Info.bmHeight, RGB(255, 255, 255));
            SelectObject(hFood2DC, hOldFood2Bmp);
            DeleteObject(hFood2DC);

            ///점수 텍스트
            
            WCHAR scoreText[100];
            wsprintf(scoreText, L"체력 : %d                                                                       점수 : %d", player_life, player_score);

            // 텍스트 배경을 투명하게
            SetBkMode(hdc, TRANSPARENT);

            // 폰트 크기 설정
            HFONT hFont = CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

            // 텍스트 그리기
            TextOut(hdc, 20, 20, scoreText, lstrlen(scoreText));

            // 폰트 복원 및 삭제
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            

            //적 = 파란색 음식 = 빨간색, 노란색, 점수 = 초록색 
            //적 하나 더 추가, 특별 음식 추가, 배경색 바뀌게

            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            //플레이어
            
            ///food1
            HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
            SelectObject(hdc, redBrush);
            
            DeleteObject(redBrush);
            ///food2
            HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
            SelectObject(hdc, yellowBrush);
            
            DeleteObject(yellowBrush);
            //적, 적2
            HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
            SelectObject(hdc, blueBrush);
            
            DeleteObject(blueBrush);
            
             
            
            //점수
            HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
            SelectObject(hdc, greenBrush);
            
            
            DeleteObject(greenBrush);

            HBRUSH eyeBrush = CreateSolidBrush(RGB(255, 255, 255));
            SelectObject(hdc, eyeBrush);
            Ellipse(hdc, player.left + 20, player.top + 20, player.left + 45, player.top + 45);
            Ellipse(hdc, player.right - 45, player.top + 20, player.right - 20, player.top + 45);
            DeleteObject(eyeBrush);

            HBRUSH pupilBrush = CreateSolidBrush(RGB(0, 0, 0));
            SelectObject(hdc, pupilBrush);
            Ellipse(hdc, player.left + 30 + lookdir, player.top + 30, player.left + 40 + lookdir, player.top + 40);
            Ellipse(hdc, player.right - 35 + lookdir, player.top + 30, player.right - 25 + lookdir, player.top + 40);
            DeleteObject(pupilBrush);
            

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        mciSendString(L"close my_bgm", NULL, 0, NULL);
        mciSendString(L"close my_hit", NULL, 0, NULL);
        mciSendString(L"close my_coin", NULL, 0, NULL);
        for (int i = 0; i < 4; i++)
        {
            if (hBgImages[i])
                DeleteObject(hBgImages);
        }
        if (hCoinBmp)
            DeleteObject(hCoinBmp);
        if (hEnemyBmp)
            DeleteObject(hEnemyBmp);
        if (hPlayerBmp)
            DeleteObject(hPlayerBmp);
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
