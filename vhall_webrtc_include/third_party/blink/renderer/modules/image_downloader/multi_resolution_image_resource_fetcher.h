// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_IMAGE_DOWNLOADER_MULTI_RESOLUTION_IMAGE_RESOURCE_FETCHER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_IMAGE_DOWNLOADER_MULTI_RESOLUTION_IMAGE_RESOURCE_FETCHER_H_

#include <memory>
#include <string>

#include "base/callback.h"
#include "base/macros.h"
#include "third_party/blink/public/mojom/fetch/fetch_api_request.mojom-blink.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/public/web/web_associated_url_loader_options.h"
#include "third_party/blink/renderer/core/execution_context/context_lifecycle_observer.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"

class SkBitmap;

namespace blink {
class KURL;
class LocalFrame;
class WebAssociatedURLLoader;
class WebURLResponse;

// A resource fetcher that returns all (differently-sized) frames in
// an image. Useful for favicons.
class MultiResolutionImageResourceFetcher {
  USING_FAST_MALLOC(MultiResolutionImageResourceFetcher);

 public:
  using Callback = base::OnceCallback<void(MultiResolutionImageResourceFetcher*,
                                           const WTF::Vector<SkBitmap>&)>;

  // This will be called asynchronously after the URL has been fetched,
  // successfully or not.  If there is a failure, response and data will both be
  // empty.  |response| and |data| are both valid until the URLFetcher instance
  // is destroyed.
  using StartCallback = base::OnceCallback<void(const WebURLResponse& response,
                                                const std::string& data)>;

  MultiResolutionImageResourceFetcher(
      const KURL& image_url,
      LocalFrame* frame,
      int id,
      mojom::blink::RequestContextType request_context,
      mojom::blink::FetchCacheMode cache_mode,
      Callback callback);

  virtual ~MultiResolutionImageResourceFetcher();

  // URL of the image we're downloading.
  const KURL& image_url() const { return image_url_; }

  // Unique identifier for the request.
  int id() const { return id_; }

  // HTTP status code upon fetch completion.
  int http_status_code() const { return http_status_code_; }

  // Called when ImageDownloaderImpl::ContextDestroyed is called.
  void Dispose();

 private:
  class ClientImpl;

  // ResourceFetcher::Callback. Decodes the image and invokes callback_.
  void OnURLFetchComplete(const WebURLResponse& response,
                          const std::string& data);

  void SetSkipServiceWorker(bool skip_service_worker);
  void SetCacheMode(mojom::FetchCacheMode mode);

  // Associate the corresponding WebURLLoaderOptions to the loader. Must be
  // called before Start. Used if the LoaderType is FRAME_ASSOCIATED_LOADER.
  void SetLoaderOptions(const WebAssociatedURLLoaderOptions& options);

  // Starts the request using the specified frame.  Calls |callback| when
  // done.
  //
  // |fetch_request_mode| is the mode to use. See
  // https://fetch.spec.whatwg.org/#concept-request-mode.
  //
  // |fetch_credentials_mode| is the credentials mode to use. See
  // https://fetch.spec.whatwg.org/#concept-request-credentials-mode
  void Start(LocalFrame* frame,
             mojom::RequestContextType request_context,
             network::mojom::RequestMode request_mode,
             network::mojom::CredentialsMode credentials_mode,
             StartCallback callback);

  // Manually cancel the request.
  void Cancel();

  Callback callback_;

  // Unique identifier for the request.
  const int id_;

  // HTTP status code upon fetch completion.
  int http_status_code_;

  // URL of the image.
  const KURL image_url_;

  std::unique_ptr<WebAssociatedURLLoader> loader_;
  std::unique_ptr<ClientImpl> client_;

  // Options to send to the loader.
  WebAssociatedURLLoaderOptions options_;

  // Request to send.
  WebURLRequest request_;

  DISALLOW_COPY_AND_ASSIGN(MultiResolutionImageResourceFetcher);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_IMAGE_DOWNLOADER_MULTI_RESOLUTION_IMAGE_RESOURCE_FETCHER_H_
