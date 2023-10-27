
// IT1Dlg.h: файл заголовка
//

#pragma once
#include <mutex>
#include "Drawer.h"
#define ID_TIMER 1 //объявляем идентификатор таймера

// Диалоговое окно CIT1Dlg
class CIT1Dlg : public CDialogEx
{
// Создание
public:
	CIT1Dlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IT1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT);//функция реакции на таймер
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//переменные для гауссовых куполов
	double A1;
	double mu1;
	double sigma1;
	double A2;
	double mu2;
	double sigma2;
	double A3;
	double mu3;
	double sigma3;
	double A0;
	double sigma0;
	double fd; //частота дискретизации 
	int N; //количество точек
	Drawer X; //рисовалка для х
	Drawer H; //рисовалка для h
	Drawer Y; //рисовалка для y
	vector <double> dots; //точки...
	afx_msg void OnBnClickedOk(); //построить выходную функцию
	void New_lambda(); //генерация случайных лямбд (стартовый вектор оптимизации)
	BOOL timer = false; //проверка на состояние таймера
	void F_otkl(); //оптимизируемая функция
	afx_msg void OnBnClickedSolve(); //решить обратную задачу (оптимизировать функцию)
	double FFunc(); //функционал
	void OnBnClickedClose(); //кнопка закрытия
	vector<double> CIT1Dlg::new_x(vector<double> hh, vector<double> lambda, int n); //х, построенный на лямбдах
	void Find_direction(int &count, double& s_F, double& temp); //найти направление шага в методе Хука-Дживса (1 этап)
	vector<double> Find_direction(vector<double> lam_temp); //найти направление (2 этап)
	double FF; //переменая, в которою записываю значение функции оптимизации и вывожу ее
	int steps; //счетчик количества шагов
	double bstep; //размер шага
	int count;
	bool next;
	afx_msg void OnBnClickedPause();
	HANDLE CalcThread;
	double otkl;
	void AddNoise(); //шум
	double noise_lvl; //уровень шума
};
