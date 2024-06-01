#include "../include/MyLibevent.hpp"
#include "../include/EventObj.hpp"
#include "cstring"
void G_CALLBACK_FUN(int fd, short ev, void *arg) {
  Sock_Obj *ptr = (Sock_Obj *)arg;
  if (ptr == NULL) {
    return;
  }

  if (ev & EV_READ) {
    // 处理事件
    ptr->CallBack_Fun(); //
  }
}
bool MyLibevent::MyLibevent_Add(int fd, Sock_Obj *pObj) {
  if (pObj == NULL) {
    return false;
  }
  struct event *pev =
      event_new(m_base, fd, EV_READ | EV_PERSIST, G_CALLBACK_FUN, pObj);
  if (pev == NULL) {
    return false;
  }
  pObj->ev = pev;
  event_add(pev, NULL);
  return true;
}
void MyLibevent::MyLibevent_Delete(Sock_Obj *pObj) {
  if (pObj != NULL) {
    event_free(pObj->ev);
  }
}
bool MyLibevent::MyLibevent_Dispatch() {
  if (m_base != NULL) {
    if (-1 == event_base_dispatch(m_base)) {
      clog << "event base dispach error " << endl;
      return false;
    }
  }
  return true;
}

