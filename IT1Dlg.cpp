
// IT1Dlg.cpp: файл реализации
//
#include "pch.h"
#include "framework.h"
#include "IT1.h"
#include "IT1Dlg.h"
#include "afxdialogex.h"
#include "math.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define M_PI 3.14159265358979323846
#define INTERVAL 20

// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CIT1Dlg


//начальная инициализация
CIT1Dlg::CIT1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IT1_DIALOG, pParent)
	, A1(16)
	, mu1(0.8)
	, sigma1(0.4)
	, A2(8)
	, mu2(2.5)
	, sigma2(0.3)
	, A3(14)
	, mu3(3.5)
	, sigma3(0.5)
	, A0(1)
	, sigma0(0.2)
	, fd(10)
	, N(48)
	, FF(0)
	, steps(0)
	, bstep (0.05)
	, otkl(0)
	, noise_lvl(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
//обмен данных с окном
void CIT1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_A1, A1);
	DDX_Text(pDX, IDC_mu1, mu1);
	DDX_Text(pDX, IDC_sigma1, sigma1);
	DDX_Text(pDX, IDC_A2, A2);
	DDX_Text(pDX, IDC_mu2, mu2);
	DDX_Text(pDX, IDC_sigma2, sigma2);
	DDX_Text(pDX, IDC_A3, A3);
	DDX_Text(pDX, IDC_mu3, mu3);
	DDX_Text(pDX, IDC_sigma3, sigma3);
	DDX_Text(pDX, IDC_A0, A0);
	DDX_Text(pDX, IDC_sigma0, sigma0);
	DDX_Text(pDX, IDC_fd, fd);
	DDX_Text(pDX, IDC_N, N);
	DDX_Text(pDX, IDC_FF, FF);
	DDX_Text(pDX, IDC_steps, steps);
	DDX_Text(pDX, IDC_step, bstep);
	DDX_Text(pDX, IDC_otkl, otkl);
	DDX_Text(pDX, IDC_noise, noise_lvl);
}

BEGIN_MESSAGE_MAP(CIT1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CIT1Dlg::OnBnClickedOk)
	ON_WM_TIMER() //для работы с таймером
	ON_BN_CLICKED(IDSOLVE, &CIT1Dlg::OnBnClickedSolve)
	ON_BN_CLICKED(IDCLOSE, &CIT1Dlg::OnBnClickedClose)
	ON_BN_CLICKED(IDPAUSE, &CIT1Dlg::OnBnClickedPause)
END_MESSAGE_MAP()



DWORD WINAPI HookeJeeves(LPVOID param); 

vector <double> x; //точки х
vector <double> h; //точки h
vector <double> y; //точки...
vector <double> lam; //точки...
vector <double> lambda;
vector<double>old_x, x_s, x_b;
int n;
bool bnext, stop(false);
double  F(0);
double Func(); //функционал
int chislo(0);
bool wait(false);



// Обработчики сообщений CIT1Dlg

BOOL CIT1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок
	//связываем область рисования к объектам класса
	X.Create(GetDlgItem(IDC_PLOT1)->GetSafeHwnd());
	H.Create(GetDlgItem(IDC_PLOT2)->GetSafeHwnd());
	Y.Create(GetDlgItem(IDC_PLOT3)->GetSafeHwnd());
	//поток для подсчета
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
	//CalcThread = CreateThread(NULL, 0, HookeJeeves, NULL, CREATE_SUSPENDED, NULL);
	SetThreadPriority(CalcThread, THREAD_PRIORITY_NORMAL);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CIT1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CIT1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		if (old_x.size() != 0) { //если х не пустой, рисуем графики

			X.DrawGrid(old_x, 1, dots, N);
			X.DrawPlot(x, 2, dots, N);
			X.DrawPlot(old_x, 1, dots, N);
			
	
			H.DrawGrid(h, 1, dots, N);
			H.DrawPlot(h, 1, dots, N);
		
			Y.DrawGrid(y, 1, dots, N);
			Y.DrawPlot(y, 1, dots, N);
			
		}
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CIT1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//гауссов купол
double Gauss(double A, double mu, double sigma, double i, double fd) {
	return A * exp(-pow((i / fd - mu), 2) / (sigma * sigma));
}

//получение выходного сигнала
void CIT1Dlg::OnBnClickedOk()
{
	x.clear();
	h.clear();
	y.clear();
	dots.clear();

	UpdateData(TRUE);
	//формируем сигнал
	for (int i = 0; i < N; i++) {
		x.push_back(Gauss(A1, mu1, sigma1, i, fd) + Gauss(A2, mu2, sigma2, i, fd) + Gauss(A3, mu3, sigma3, i, fd));
		dots.push_back(i);
	}
	//формируем дельта-функцию
	for (int i = 0; i < N; i++) {
		if (i < N / 2.) h.push_back(Gauss(A0, 0, sigma0, i, fd));
		else h.push_back(Gauss(A0, 0, sigma0, N - i - 1, fd));
	}
	//выходная функция
	for (int i = 0; i < N; i++) {
		double yy = 0;
		for (int j = 0; j < N; j++) {
			yy += x[j] * h[abs(j - i)];
		}
		y.push_back(yy);
	}

	//рисуем графики
	old_x = x;
	AddNoise();
	RedrawWindow();
	
}

//новые лямбды
void CIT1Dlg::New_lambda() {
	lam.clear();
	x_s.clear();
	x_b.clear();
	lambda.clear();
	x.clear();
	for (int i = 0; i < N; i++) {
		lam.push_back(0);
		x_s.push_back(0);
		x_b.push_back(0);
		lambda.push_back(0);
		x.push_back(0);
	}
}

//решение обратной задачи
void CIT1Dlg::OnBnClickedSolve()
{
	//если таймер не запущен
	if (!timer) {
		UpdateData();
		srand(time(NULL));
		n = N;
		if (y.size() == 0) OnBnClickedOk(); //проверка на пустоту выходного сигнала 
		New_lambda();
		x = new_x(h, lam, N);
		X.DrawGrid(old_x, 1, dots, N);
		X.DrawPlot(old_x, 1, dots, N);
		X.DrawPlot(x, 2, dots, N);
		steps = 0;
		bnext = false;
		stop = false;
		chislo = 0;
		CalcThread = CreateThread(NULL, 0, HookeJeeves, &bstep, 0, NULL);
		SetTimer(ID_TIMER, INTERVAL, NULL);
		timer = true;
	}
	//если таймер запущен, выключить его по нажатию кнопки
	else {
		KillTimer(ID_TIMER);
		timer = false;
		stop = true; // поток закрывается
		//New_lambda();
	}

}

//х по новым лямбдам
vector<double> CIT1Dlg::new_x(vector<double> hh, vector<double> lambda, int n) {
	vector<double> xx;
	for (int m = 0; m < n; m++) {
		double sum = 0;
		for (int i = 0; i < n; i++) {
			sum -= hh[abs(i - m)] * lambda[i];
		}
		xx.push_back(exp(-1 + sum));
	}
	return xx;
}

//квадратичное отклонение
void CIT1Dlg::F_otkl(){
	otkl = 0;
	for (int i = 0; i < N; i++){
		otkl += pow(old_x[i] - x[i], 2);
	}
}

void CIT1Dlg::AddNoise(){ //уровень шума задаётся в долях 1 = 100% (от сигнала) 
	if (noise_lvl <= 1e-5) return;
	double beta(0),sumg(0), sumy(0);
	double* g = new double[N];
	for (int j = 0; j < N; j++){
		g[j] = 0;
		for (int i = 0; i < 12; i++){
			g[j] += rand() - RAND_MAX/2;
		}
		sumy += y[j] * y[j];
		sumg += g[j] * g[j];
	}
	beta = sqrt(noise_lvl*sumy/sumg);
	for (int i = 0; i < N; i++){
		y[i] += beta * g[i];
	}
}

//эта функция вызывается таймером
void CIT1Dlg::OnTimer(UINT) {
	if (stop){
		timer = false;
		KillTimer(ID_TIMER);
	}
	wait = true;
	FF = F;
	steps = chislo;
	x = new_x(h, lambda, N);
	F_otkl();
	wait = false;
	UpdateData(FALSE);
	X.DrawGrid(old_x, 1, dots, N);
	X.DrawPlot(old_x, 1, dots, N);
	X.DrawPlot(x, 2, dots, N);	
}

//выбираем направление для шага
void Find_direction(int& count, double& s_F, double& temp, double& step) {
	count = 0;
	for (int i = 0; i < n; i++) {
		//выбираем направление 
		count++;
		lam[i] += step; //шагаем
		if (Func() >= s_F) { //если функция не уменьшилась, значит, шаг неуспешный

			lam[i] -= 2 * step; //шагаем в противоположную сторону
			if (Func() >= s_F) { //если опять неудачно, возвращаемся в исходную позицию
				count--;
	
				lam[i] += step;
			}
		}
		s_F = Func();
	}
	temp = Func();
}

//шаг по направлению
vector<double> Find_direction(vector<double> lam_temp) {
	vector<double> direction, llam(lam);
	for (int i = 0; i < n; i++) {
		direction.push_back(2 * llam[i] - lam_temp[i]);
	}
	return direction;
}

//функция, которая вызывается потоком
DWORD WINAPI HookeJeeves(LPVOID param) {
	double step = *(double*)param;
	double s_F(0),temp (0);
	int count(0);
	chislo = 0;
	
	DWORD exit(0);
	DWORD sleept(5);
	do {
		if (stop) ExitThread(exit);
		if (!bnext) {
			s_F = Func(); //стартовое значение функции оптимизации F(x0) - это первый этап
			temp = s_F; //временная переменная, F(x0)
			count = 1;
			x_b = lam; //сохраняем хb = x0
			//2 этап 
			Find_direction(count, s_F, temp, step);//выбираем направление, получаем xs; temp = F(xs)
			x_s = lam; //сохраняем хs
			if (count == 0) { //уменьшаем шаг в 2 и выходим, если вокруг не оказалось точек лучше
				step /= 2;
				chislo++;
				continue;
			}
		}
		do {
			if (stop) ExitThread(exit);
			s_F = temp; //сохраняем F(xs) 
			lam = Find_direction(x_b); //xp = 2*xs - xb
			x_b = x_s; //xb = xs
			x_s = lam; //xs = xp
			double F_xp = Func();//сохраняем F(xp)
			Find_direction(count, F_xp, temp, step); //xq, F(xq)
			if (temp < s_F && count > 0) {
				x_s = lam; //сохраним xq, если в хp есть куда шагать, и повторим шаг на следующей итерации
				bnext = true;
				if (wait) Sleep(sleept);
				chislo++;
				F = temp;
				if (wait) Sleep(sleept);
				lambda = lam;
				continue;
			}
			//иначе вернемся в хs
			else {
				lam = x_b;
				bnext = false;
				if (wait) Sleep(sleept);
				chislo++;
				F = temp;
				if (wait) Sleep(sleept);
				lambda = lam;
				break;
			}
		} while (temp < s_F);
	} while (step > 1e-6 && chislo < 500 && temp > 0.5); // условия остановки: шаг (число), размеры шага, функционал
	stop = true;
	return 0;

}

//оптимизируемая функция (функционал) - глобальная ф-ия
double Func()
{
	vector<double> sumk(n);
	double summ(0);
	double sumi(0);
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < n; i++) {

			sumi += lam[i] * h[abs(i - k)];
		}
		sumk[k] = sumi;
		sumi = 0;
	}
	for (int m = 0; m < n; m++) {
		double sum(0);
		for (int k = 0; k < n; k++) {
			sum += h[abs(m - k)] * exp(-1 - sumk[k]);
		}
		summ += pow(y[m] - sum, 2);
	}
	return summ;
}

//функция класса (= метод)
double CIT1Dlg::FFunc()
{
	vector<double> sumk(n);
	double summ(0);
	double sumi(0);
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < n; i++) {

			sumi += lambda[i] * h[abs(i - k)];
		}
		sumk[k] = sumi;
		sumi = 0;
	}
	for (int m = 0; m < n; m++) {
		double sum(0);
		for (int k = 0; k < n; k++) {
			sum += h[abs(m - k)] * exp(-1 - sumk[k]);
		}
		summ += pow(y[m] - sum, 2);
	}
	return summ;
}

void CIT1Dlg::OnBnClickedClose()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
	// или
	AfxGetMainWnd()->DestroyWindow();
}

void CIT1Dlg::OnBnClickedPause() //кнопка пауза/продолжить
{
	if (!timer && !stop) {
		SetTimer(ID_TIMER, INTERVAL, NULL);
		timer = true;
		ResumeThread(CalcThread);
	}
	//если таймер запущен, выключить его по нажатию кнопки
	else if(!stop){
		SuspendThread(CalcThread);
		KillTimer(ID_TIMER);
		timer = false;
	}
}
