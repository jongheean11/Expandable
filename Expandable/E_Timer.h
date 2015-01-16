

class E_Timer
{
public:
	E_Timer();
	void setInterval(int interval);
	void Start();
	bool Done();
private:
	int startTick;
	int interval;
	bool flag;
};
