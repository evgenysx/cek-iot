#ifndef _CEK_ESP_BUFFER_SERIALIZER_H_
#define _CEK_ESP_BUFFER_SERIALIZER_H_

#include <WString.h>
#include <Arduino.h>

struct ISerialize {
    const virtual size_t getSize() = 0;
    const virtual byte* getBytes() = 0;
};

class BufferSerializer : public ISerialize{
    public:
        /**
         * сериализуем строку
        */
        bool put(String& value);
        String readString();
        /**
         * сериализуем число
        */
        bool put(size_t value);
        size_t readUint();
        //
        /**
         * сериализуем BufferSerializer
        */
        bool put(BufferSerializer& buf);
        BufferSerializer readBuf();
        //
        const size_t getSize() override;
        const byte* getBytes() override;


        //
        BufferSerializer();
        BufferSerializer(size_t capicity);
        virtual ~BufferSerializer();

        //
        void setOffset(size_t offset);
    private:
        /**
         * проверяет и, при необходимости, добавляет памяти в буфер
         * szData - размер добавляемого блока
         * */ 
        bool chkAddMemory(size_t szData);
    private:
        // сдвиг; реально использованное количество памяти
        size_t offset; 
        byte* data;
        // емкость буфера. Общее количество выделенной памяти 
        size_t capicity;
};

#endif