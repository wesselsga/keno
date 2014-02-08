#pragma once

#include "window.h"

namespace screen {

//
// BCM Host window implementation for Raspberry PI
//
class BcmWindow : public Window
{
   struct _ctx { };
public:
   explicit BcmWindow(const _ctx&,const std::weak_ptr<Channel>&);	
	virtual ~BcmWindow();

	static std::shared_ptr<BcmWindow> create(
            const std::string&,
            const std::weak_ptr<Channel>&);

	void*   handle() const { return _eglw; }
   void*   display() const { return nullptr; }
	void    show() const;
	void    update() const;
	void    close();
	int32_t pump();

private:
   BcmWindow()=delete;
	BcmWindow(const BcmWindow&)=delete;
	const BcmWindow& operator=(const BcmWindow&) = delete;

	void* _eglw;		

};

};
