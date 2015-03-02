/******************************************************************************
 **  Copyright (c) 2007-2015, Calaos. All Rights Reserved.
 **
 **  This file is part of Calaos.
 **
 **  Calaos is free software; you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation; either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Calaos is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Foobar; if not, write to the Free Software
 **  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 **
 ******************************************************************************/
#ifndef CALAOS_URL_UTILS_H
#define CALAOS_URL_UTILS_H

#include <stdint.h>

#include <Utils.h>
#include <sigc++/sigc++.h>
#include <Ecore.h>
#include <Ecore_Con.h>

using namespace Utils;

//File Downloader thread
class UrlDownloader
{

private:
    enum RequestType {HTTP_GET, HTTP_PUT, HTTP_POST, HTTP_DELETE};

    RequestType m_requestType = HTTP_POST;

    bool m_auth = false;
    string m_user;
    string m_password;

    // EcoreCon events handlers
    Ecore_Event_Handler *m_completeHandler;
    Ecore_Event_Handler *m_dataHandler;
    Ecore_Event_Handler *m_progressHandler;

    // Ecore Con handle
    Ecore_Con_Url *m_urlCon = nullptr;

    // Signals/Slots
    sigc::signal<void, int> m_signalComplete;
    sigc::signal<void, Eina_Binbuf*, int> m_signalCompleteData;
    sigc::signal<void, double, double> m_signalProgress;
    sigc::signal<void, int, unsigned char*> m_signalData;

    sigc::connection m_connectionComplete;
    sigc::connection m_connectionCompleteData;
    sigc::connection m_connectionProgress;
    sigc::connection m_connectionData;

    // File pointer to pass to ecore when a destination is given
    FILE *m_file = nullptr;
    // fd of the file when a fd is given
    int m_fd = -1;
    // Url to contact
    string m_url = "";
    // Destination where to store the result
    string m_destination = "";
    // Message Body
    string m_bodyData = "";
    // Content type to be send
    string m_postContentType = "";

    //Common function for starting download of url
    bool start();

public:
    // BinBuf pointer containing data downloaded
    Eina_Binbuf *m_downloadedData = NULL;

    // Constructor
    UrlDownloader(string url);

    // Setter for private members
    void bodyDataSet(string bodyData) {m_bodyData = bodyData;}
    void postContentTypeSet(string postContentType) {m_postContentType = postContentType;}
    void destinationSet(string destination) {m_destination = destination;}
    void authSet(string user, string password) {m_user = user; m_password = password; m_auth = true;}
    void authUnSet() {m_auth = false;}
    void fdSet(int fd) {m_fd = fd;}

    // Setter for slots
    void completeCbSet(sigc::slot<void, int> slot) {
        m_connectionComplete.disconnect();
        m_connectionComplete = m_signalComplete.connect(slot);
    }

    void completeDataCbSet(sigc::slot<void, Eina_Binbuf*, int> slot) {
        m_connectionCompleteData.disconnect();
        m_connectionCompleteData = m_signalCompleteData.connect(slot);
    }

    void progressCbSet(sigc::slot<void, double, double> slot) {
       m_connectionProgress.disconnect();
       m_connectionProgress = m_signalProgress.connect(slot);
    }

   void dataCbSet(sigc::slot<void, int, unsigned char*> slot) {
        m_connectionData.disconnect();
        m_connectionData = m_signalData.connect(slot);
    }

   // HTTP Post
   bool httpPost();

   // HTTP Get
   bool httpGet();

   // HTTP Put
   bool httpPut();

   // HTTP Delete
   bool httpDelete();

   bool httpGet(string destination,
                 sigc::slot<void, int> slot);
   bool httpGet(sigc::slot<void, Eina_Binbuf*, int> slot);

   bool httpPost(string bodyData, sigc::slot<void, Eina_Binbuf*, int> slot);
   bool httpGet(string bodyData, sigc::slot<void, Eina_Binbuf*, int> slot);
   bool httpPut(string bodyData, sigc::slot<void, Eina_Binbuf*, int> slot);

   void completeCb(int status);
   void dataCb(unsigned char *data, int size);
   void progressCb(double now, double total);

   ~UrlDownloader();

};

#endif