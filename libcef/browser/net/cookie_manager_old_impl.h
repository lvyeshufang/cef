// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_NET_COOKIE_MANAGER_OLD_IMPL_H_
#define CEF_LIBCEF_BROWSER_NET_COOKIE_MANAGER_OLD_IMPL_H_

#include <set>

#include "include/cef_cookie.h"
#include "libcef/browser/request_context_impl.h"
#include "libcef/browser/thread_util.h"

#include "base/files/file_path.h"
#include "net/cookies/cookie_monster.h"

// Implementation of the CefCookieManager interface.
class CefCookieManagerOldImpl : public CefCookieManager {
 public:
  CefCookieManagerOldImpl();
  ~CefCookieManagerOldImpl() override;

  // Must be called immediately after this object is created when |is_blocking|
  // is false.
  void Initialize(CefRefPtr<CefRequestContextImpl> request_context,
                  const CefString& path,
                  bool persist_session_cookies,
                  CefRefPtr<CefCompletionCallback> callback);

  // Executes |callback| either synchronously or asynchronously with the
  // CookieStoreGetter when the cookie store object is available. If
  // |task_runner| is NULL the callback will be executed on the originating
  // thread. CookieStoreGetter can only be executed on, and the resulting cookie
  // store object can only be accessed on, the IO thread.
  typedef base::Callback<net::CookieStore*()> CookieStoreGetter;
  typedef base::Callback<void(const CookieStoreGetter&)> CookieStoreCallback;
  void GetCookieStore(scoped_refptr<base::SingleThreadTaskRunner> task_runner,
                      const CookieStoreCallback& callback);

  // Returns the existing cookie store object. Logs an error if the cookie
  // store does not yet exist. Must be called on the IO thread.
  net::CookieStore* GetExistingCookieStore();

  // CefCookieManager methods.
  void SetSupportedSchemes(const std::vector<CefString>& schemes,
                           bool include_defaults,
                           CefRefPtr<CefCompletionCallback> callback) override;
  bool VisitAllCookies(CefRefPtr<CefCookieVisitor> visitor) override;
  bool VisitUrlCookies(const CefString& url,
                       bool includeHttpOnly,
                       CefRefPtr<CefCookieVisitor> visitor) override;
  bool SetCookie(const CefString& url,
                 const CefCookie& cookie,
                 CefRefPtr<CefSetCookieCallback> callback) override;
  bool DeleteCookies(const CefString& url,
                     const CefString& cookie_name,
                     CefRefPtr<CefDeleteCookiesCallback> callback) override;
  bool FlushStore(CefRefPtr<CefCompletionCallback> callback) override;

  // Set the schemes supported by |cookie_monster|.
  static void SetCookieMonsterSchemes(net::CookieMonster* cookie_monster,
                                      const std::vector<std::string>& schemes,
                                      bool include_defaults);

 private:
  // Execute |method| on the IO thread once the request context is available.
  void RunMethodWithContext(
      const CefRequestContextImpl::RequestContextCallback& method);

  void InitWithContext(
      CefRefPtr<CefCompletionCallback> callback,
      scoped_refptr<CefURLRequestContextGetter> request_context);
  void SetSupportedSchemesWithContext(
      const std::vector<std::string>& schemes,
      bool include_defaults,
      CefRefPtr<CefCompletionCallback> callback,
      scoped_refptr<CefURLRequestContextGetter> request_context);
  void GetCookieStoreWithContext(
      scoped_refptr<base::SingleThreadTaskRunner> task_runner,
      const CookieStoreCallback& callback,
      scoped_refptr<CefURLRequestContextGetter> request_context);

  void SetSupportedSchemesInternal(const std::vector<std::string>& schemes,
                                   bool include_defaults,
                                   CefRefPtr<CefCompletionCallback> callback);
  void VisitAllCookiesInternal(CefRefPtr<CefCookieVisitor> visitor,
                               const CookieStoreGetter& cookie_store_getter);
  void VisitUrlCookiesInternal(const CefString& url,
                               bool includeHttpOnly,
                               CefRefPtr<CefCookieVisitor> visitor,
                               const CookieStoreGetter& cookie_store_getter);
  void SetCookieInternal(const GURL& url,
                         const CefCookie& cookie,
                         CefRefPtr<CefSetCookieCallback> callback,
                         const CookieStoreGetter& cookie_store_getter);
  void DeleteCookiesInternal(const GURL& url,
                             const CefString& cookie_name,
                             CefRefPtr<CefDeleteCookiesCallback> callback,
                             const CookieStoreGetter& cookie_store_getter);
  void FlushStoreInternal(CefRefPtr<CefCompletionCallback> callback,
                          const CookieStoreGetter& cookie_store_getter);

  // Context that owns the cookie monster.
  CefRefPtr<CefRequestContextImpl> request_context_;
  scoped_refptr<CefURLRequestContextGetter> request_context_impl_;

  IMPLEMENT_REFCOUNTING_DELETE_ON_IOT(CefCookieManagerOldImpl);
};

#endif  // CEF_LIBCEF_BROWSER_NET_COOKIE_MANAGER_OLD_IMPL_H_
