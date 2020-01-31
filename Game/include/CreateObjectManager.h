#pragma once

class ICreateObject
{
public:
	ICreateObject() {
		//���X�g�ɓo�^	
		m_s_allObjects.emplace_back(this);
	}
	virtual ~ICreateObject(){
		if (m_s_isAllDestroyMode) {
			return;
		}
		//���X�g����폜
		for (auto it = m_s_allObjects.begin(), e = m_s_allObjects.end(); it != e; ++it) {
			if (*it == this) {
				m_s_allObjects.erase(it);
				break;
			}
		}
	}

	//�������Ă�����̂����ׂč폜
	static inline void AllDestroy() {
		m_s_isAllDestroyMode = true; 		
		
		for (ICreateObject* p : m_s_allObjects) {
			delete p;
		}
		m_s_allObjects.clear();

		m_s_isAllDestroyMode = false;
	}

private:
	static inline bool m_s_isAllDestroyMode = false;
	static std::list<ICreateObject*> m_s_allObjects;
};

