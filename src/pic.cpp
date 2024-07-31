#include <iostream>
#include <fstream>
#include <string>
#include <strstream>
#include <map>
#include <vector>
#include <regex>
#include <iostream>
#include <iomanip>

#include "my_sys.h"

class BitMapPixUnit
{
	public:
		BitMapPixUnit(byte *addr, size_t size, size_t step = 3)
			:m_addr(addr), m_size(size), m_step(step)
		{
			m_step = m_step ? m_step : 3;
		}

		~BitMapPixUnit(){
		}

		byte& operator[](size_t idx)
		{
			return m_addr[idx * m_step];
		}

		int Init(byte base)
		{
			size_t i;
			size_t step = m_step ? m_step : 1;
			for (i = 0; i < m_size; i += 1){
				m_addr[i * step] = base + i;
			}
			return 0;
		}
		void Dump(std::ostream &out, size_t lnsz){
			size_t i;
			size_t step = m_step ? m_step : 1;
			for (i = 0; i < m_size; i += 1){
				if (i % lnsz == 0) {
					out << std::endl;		
				}
				out << std::setiosflags(std::ios::uppercase) << std::hex << std::setfill('0') << std::setw(8) <<  m_addr[i * step];
				out << " ";
			}
			out << std::endl;
		}

	private:
		byte *m_addr;
		size_t m_size;
		size_t m_step;
};

class IBitMapPix
{
	public:
		IBitMapPix(byte *addr, size_t size){}		
		virtual ~IBitMapPix(){}
		virtual byte& Access(size_t idx, size_t offset) = 0;
		virtual int Init() = 0;
		virtual void Dump(std::ostream &out, size_t lnsz) = 0;
	protected:
		byte *m_addr;
		size_t m_size;
};

class RgbBitMapPix:public IBitMapPix
{
	public:
		RgbBitMapPix(byte *addr, size_t size)
		:IBitMapPix(addr, size),
		m_r(addr, size, 3),m_g(addr + 1, size, 3),m_b(addr + 2, size, 3){}	
		virtual ~RgbBitMapPix(){}
		virtual byte& Access(size_t idx, size_t offset){
			switch (offset)
			{
			case 0:
				return m_r[idx];
			case 1:
				return m_g[idx];
			case 2:
				return m_b[idx];
			default:
				return m_r[idx];
			} 
		}
		virtual int Init(){
			m_r.Init(0xFF00);
			m_g.Init(0xEE00);
			m_b.Init(0xDD00);
			return 0;
		}

		void Dump(std::ostream &out, size_t lnsz) {
			m_r.Dump(out, lnsz);
			m_g.Dump(out, lnsz);
			m_b.Dump(out, lnsz);
		}

	private:
		BitMapPixUnit m_r;
		BitMapPixUnit m_g;
		BitMapPixUnit m_b;
};

class YuvBitMapPix:public IBitMapPix
{
	public:
		YuvBitMapPix(byte *addr, size_t size)
		:IBitMapPix(addr, size),
		m_y(addr, size, 1),m_u(addr + size, size, 2),m_v(addr + size + 1, size, 2){}	
		virtual ~YuvBitMapPix(){}
		virtual byte& Access(size_t idx, size_t offset){
			switch (offset)
			{
			case 0:
				return m_y[idx];
			case 1:
				return m_u[idx];
			case 2:
				return m_v[idx];
			default:
				return m_y[idx];
			} 
		}
		virtual int Init(){
			m_y.Init(0xAA00);
			m_u.Init(0xBB00);
			m_v.Init(0xCC00);
			return 0;
		}

		void Dump(std::ostream &out, size_t lnsz) {
			m_y.Dump(out, lnsz);
			m_u.Dump(out, lnsz);
			m_v.Dump(out, lnsz);
		}

	private:
		BitMapPixUnit m_y;
		BitMapPixUnit m_u;
		BitMapPixUnit m_v;
};

class BitMap
{
	public:
		BitMap(size_t width, size_t height, int type = 0)
		:m_width(width), m_height(height)
		{
			m_addr = new byte[width * height * 3];
			if (type == 0) {
				m_pix = new RgbBitMapPix(m_addr, (width * height));
			} else {
				m_pix = new YuvBitMapPix(m_addr, (width * height));
			}
		}
		~BitMap(){
			delete m_pix;
			delete[] m_addr;
		}
		int Init()
		{
			m_pix->Init();
			return 0;
		}
		void Dump(std::ostream &out){
			m_pix->Dump(out, m_width);
		}
		
	private:
		IBitMapPix *m_pix;
		size_t m_width;
		size_t m_height;
		byte *m_addr;
		size_t m_step;
};

int PicMain( int argc, char* argv[] )
{
	BitMap bm(8, 8, 1);
	bm.Init();
	bm.Dump(std::cout);
	return 0;
}
