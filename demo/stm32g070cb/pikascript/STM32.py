import PikaStdDevice

class GPIO(PikaStdDevice.GPIO):
    # override
    def platformHigh():
        pass

    # override
    def platformLow():
        pass

    # override
    def platformEnable():
        pass

    # override
    def platformDisable():
        pass

    # override
    def platformSetMode(mode: str):
        pass


class Time(PikaStdDevice.Time):
    # override
    def sleep_s(s: int):
        pass

    # override
    def sleep_ms(ms: int):
        pass


class ADC(PikaStdDevice.ADC):
    # override
    def platformEnable(pin: str):
        pass

    # override
    def platformRead(pin: str) -> float:
        pass


class UART(PikaStdDevice.UART):
    # override
    def platformEnable(id: int, baudRate: int):
        pass

    # override
    def platformWrite(id: int, data: str):
        pass

    # override
    def platformRead(id: int, length: int) -> str:
        pass
